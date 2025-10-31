#include <ctime>
#include <fstream>
#include <iomanip>
#include <list>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "../inc/core.h"
#include "../inc/os.h"
#include "../inc/scheduler.h"

OS::OS (DisplayManager &dm, Config &config)
    : dm(dm),                                    // Display manager
      config(config),                            // Config values
      scheduler(Scheduler(config.getScheduler(), // Scheduler
                          config.getQuantumCycles()))
{
    for (int i = 0; i < config.getNumCPU(); i++) {
        this->cores.push_back(Core(i));
    }
};

void OS::incrementCycles () { this->cycle++; }

void OS::resetCycles () { this->cycle = 0; }

void OS::ls (bool writeReport)
{
    // Lock the mutex first to avoid segfaults when accessing a pre-empted
    // process (which would be a null pointer)
    std::lock_guard<std::mutex> lock(this->mutex);

    // Set up a string stream to print to
    std::stringstream ss;

    // Number of cores
    int nCores = this->cores.size();

    // Calculate number of running cores
    std::list<int> runningCoreIds;
    for (int i = 0; i < nCores; i++) {
        if (cores[i].isRunning())
            runningCoreIds.push_back(cores[i].getId());
    }
    int nRunningCores = runningCoreIds.size();

    // CPU utilization
    float cpuUtil = nRunningCores * 100.0 / nCores;

    // Print system information to the stream
    ss << "CPU utilization: " << cpuUtil << "\%\n"
       << "Cores used: " << nRunningCores << "\n"
       << "Cores available: " << nCores - nRunningCores << "\n\n";

    // TODO: Clean up alignment with std::left and std::setw in this section

    // Running processes
    ss << "Running processes:\n";
    for (int i : runningCoreIds) {
        // Process has a name
        ss << std::left << std::setw(10)
           << cores[i].getProcessReference()->getName();

        // Formatted process start time
        std::time_t now = cores[i].getProcessReference()->getStartTime();
        ss << std::put_time(std::localtime(&now), " (%m/%d/%Y %H:%M:%S)");

        // Core ID
        ss << " Core: " << std::setw(5) << std::to_string(cores[i].getId())
           << "     ";

        // Progress
        ss << std::right << std::setw(10)
           << cores[i].getProcessReference()->getProgramCounter() << " / "
           << cores[i].getProcessReference()->getTotalCycles() << "\n";
    }

    // TODO: Use process name instead of ID
    // Finished processes
    ss << "\nFinished processes:\n";
    for (std::pair<std::string, int> finishedProcess :
         this->finishedProcesses) {
        // Process name
        ss << finishedProcess.first << "      FINISHED     ";

        // Progress
        ss << finishedProcess.second << " / " << finishedProcess.second << "\n";
    }

    this->dm.clearOutputWindow();

    // report-util
    if (writeReport) {
        std::ofstream report("csopesy-log.txt");
        report << ss.str();
        report.close();
        this->dm._mvwprintw(0, 0, "Report written to csopesy-log.txt");
    } // screen -ls
    else {
        this->dm.clearOutputWindow();
        this->dm._mvwprintw(0, 0, "%s", ss.str().c_str());
    }
}

void OS::screenR (std::string processName)
{
    // Lock mutex
    std::lock_guard<std::mutex> lock(this->mutex);

    // Flag for if the process was found
    bool found = false;

    this->dm.clearOutputWindow();

    for (Core &core : this->cores) {
        // On a running core, check if process name matches
        if (core.isRunning() &&
            core.getProcessReference()->getName() == processName) {

            // Copy the process pointer to OS for access with process-smi
            this->loadedProcess.reset();
            this->loadedProcess = core.getProcess();

            // Show default message for this screen
            this->showDefaultProcessScreenMessage();

            return;
        }
    }

    // TODO: Search in the ready queue. Big problem because std::queue cannot be
    // iterated over, and we can't easily make a copy of it because we'd have to
    // call std::move() on each std::shared_ptr in the queue

    const auto &readyQueue = this->scheduler.getReadyQueue();

    for (const auto &process : readyQueue) {
        if (process->getName() == processName) {
            // Copy the process pointer to OS
            this->loadedProcess.reset();
            this->loadedProcess = process;

            // Show default message for this screen
            this->showDefaultProcessScreenMessage();

            return;
        }
    }

    this->dm._mvwprintw(0, 0, "Process '%s' not found.", processName.c_str());
}

void OS::showDefaultProcessScreenMessage ()
{
    this->dm._mvwprintw(
        0, 0,
        // Default message
        "You are on the screen for process %s. Type help to get a list of "
        "valid commands.",
        this->loadedProcess->getName().c_str());
}

void OS::screenS (std::string processName)
{
    // Lock mutex before creating a new process
    std::unique_lock<std::mutex> lock(this->mutex);

    // TODO: Use a global counter for process IDs
    std::shared_ptr<Process> process(new Process(
        this->processAutoId, processName,
        this->config.getMinIns() + rand() % (this->config.getMaxIns() -
                                             this->config.getMinIns() + 1)));

    // Copies the shared pointer to loadedProcess
    this->loadedProcess = process;

    // Show default message for this screen
    this->showDefaultProcessScreenMessage();

    // Transfers ownership to scheduler queue
    this->scheduler.addProcess(process);
}

void OS::processSMI ()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->dm.clearOutputWindow();

    // Print process state information
    this->dm._mvwprintw(0, 0, "%s",
                        this->loadedProcess->getStateAsString().c_str());
}

void OS::setGenerateDummyProcesses (bool value)
{
    this->generateDummyProcesses = value;
}

bool OS::isRunning () { return this->running; }

void OS::exit ()
{
    this->running = false;

    // Lock the mutex. TODO: I don't think this is actl needed
    // std::lock_guard<std::mutex> lock(this->mutex);

    if (this->thread.joinable()) {
        this->thread.join();
    }
}
