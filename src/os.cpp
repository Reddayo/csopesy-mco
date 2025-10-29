#include <list>
#include <mutex>
#include <thread>

#include "../inc/core.h"
#include "../inc/os.h"
#include "../inc/scheduler.h"

OS::OS (DisplayManager &dm, Config &config)
    : dm(dm),                                         // Display manager
      config(config),                                 // Config values
      scheduler(Scheduler(config.getScheduler(),
                          config.getQuantumCycles())) // Scheduler
{
    for (int i = 0; i < config.getNumCPU(); i++) {
        this->cores.push_back(Core(i));
    }
};

void OS::run ()
{
    this->running = true;

    this->thread = std::thread([this] () {
        int processId = 0;

        this->resetCycles();

        while (this->running) {
            std::lock_guard<std::mutex> lock(this->mutex);

            // Create a new process and add it to the scheduler's ready queue
            // every n cycles (dictated by batch-process-freq)
            if (this->generateDummyProcesses &&
                this->cycle % this->config.getBatchProcessFreq() == 0) {

                // Create a new process and wrap it in a unique pointer
                std::unique_ptr<Process> process(new Process(
                    processId, this->config.getMinIns() +
                                   rand() % (this->config.getMaxIns() -
                                             this->config.getMinIns() + 1)));

                // Transfer ownership of the unique pointer to the scheduler
                // queue This calls std::move() internally
                this->scheduler.addProcess(process);
                processId++;
            }

            // Pre-empt processes if RR algorithm
            if (this->config.getScheduler() == RR) {
                for (Core &core : this->cores) {
                    if (core.isRunning()) {
                        // Get a reference to the unique pointer owned by the
                        // core. Ownership is not relinquished until we know it
                        // must be pre-empted.
                        std::unique_ptr<Process> &process = core.getProcess();

                        // TODO: Better way than resetting elapsed cycles?
                        if (process->getElapsedCycles() ==
                            this->config.getQuantumCycles()) {

                            process->resetElapsedCycles();

                            core.setRunning(false);
                            process->setState(READY);

                            // Relinquishes ownership of pointer to the ready
                            // queue
                            this->scheduler.addProcess(process);
                        }
                    }
                }
            }

            // Dispatch processes to any available cores
            for (Core &core : this->cores) {
                if (!(this->scheduler.isQueueEmpty())) {
                    if (!core.isRunning()) {
                        this->scheduler.dispatch(core);

                        // When a core gets a process, its state becomes RUNNING
                        // TODO: Handle this within dispatch()?
                        core.setRunning(true);
                    }
                }
            }

            // Execute each core's process in a separate thread
            for (Core &core : this->cores) {
                if (core.isRunning()) {
                    // Create new thread
                    std::thread thread = std::thread([this, &core] () {
                        // Get a reference to the unique pointer owned by the
                        // Core. Does NOT relinquish ownership.
                        std::unique_ptr<Process> &process = core.getProcess();

                        // Check if process is running first
                        if (process->getState() == RUNNING) {

                            // If process is NOT in a busy waiting state...
                            if (process->getRemainingBusyWaitingCycles() == 0) {
                                process->execute(dm);

                                // When process terminates, core stops running
                                if (process->getState() == TERMINATED) {
                                    // TODO: Move to terminated processes?
                                    core.setRunning(false);
                                    // Destroy the Process
                                    process.reset();
                                } else {
                                    // If process hasn't terminated yet, set a
                                    // busy-waiting timer until next instruction
                                    process->setBusyWaitingCycles(
                                        this->config.getDelaysPerExec());
                                    process->incrementElapsedCycles();
                                }

                            } else {
                                // Delay execution until busy-waiting timer out
                                process->decrementBusyWaitingCycles();
                                process->incrementElapsedCycles();
                            }
                        }
                    });

                    // We will loop through the threads queue later
                    this->threads.push(std::move(thread));
                }
            }

            // Join all execution threads
            while (!this->threads.empty()) {
                std::thread &thread = this->threads.front();
                thread.join();
                this->threads.pop();
            }

            // Proceed to next cycle
            this->cycle++;
        }
    });
}

void OS::incrementCycles () { this->cycle++; }

void OS::resetCycles () { this->cycle = 0; }

// TODO: Cleanup
void OS::ls ()
{
    // Lock the mutex first to avoid segfaults when accessing a pre-empted
    // process (which would be a null pointer)
    std::lock_guard<std::mutex> lock(this->mutex);

    std::string status_string;

    // Number of cores
    int nCores = this->cores.size();

    // Running cores
    std::list<int> runningCoreIds;
    for (int i = 0; i < nCores; i++) {
        if (cores[i].isRunning())
            runningCoreIds.push_back(cores[i].getId());
    }

    // Number of running cores
    int nRunningCores = runningCoreIds.size();

    // CPU utilization
    float cpuUtil = nRunningCores * 100.0 / nCores;

    // Status string
    status_string =
        "CPU utilization: " + std::to_string(cpuUtil) + "\%\n" +
        "Cores used: " + std::to_string(nRunningCores) + "\n" +
        "Cores available: " + std::to_string(nCores - nRunningCores) + "\n\n";

    // Running processes
    status_string += "Running processes:\n";
    for (int i : runningCoreIds) {
        // Process name is same as ID .....I think
        status_string += "process ";
        status_string += std::to_string(cores[i].getProcess()->getId());

        // Core name is same as ID ............I think
        status_string += "      Core ";
        status_string += std::to_string(cores[i].getId());

        status_string += "     ";
        // Progress
        status_string +=
            std::to_string(cores[i].getProcess()->getProgramCounter());
        status_string += " / ";
        status_string +=
            std::to_string(cores[i].getProcess()->getTotalCycles());

        status_string += "\n";
    }

    // Finished processes
    status_string += "\n";
    status_string += "Finished processes:\n";

    for (std::pair<int, int> finishedProcess : this->finishedProcesses) {
        // Process name
        status_string += "process ";
        status_string += std::to_string(finishedProcess.first);

        // Finished
        status_string += "      FINISHED     ";

        // Progress
        status_string += std::to_string(finishedProcess.second);
        status_string += " / ";
        status_string += std::to_string(finishedProcess.second);

        status_string += "\n";
    }

    this->dm.clearOutputWindow();
    this->dm._mvwprintw(0, 0, "%s", status_string.c_str());
}

void OS::setGenerateDummyProcesses (bool value)
{
    this->generateDummyProcesses = value;
}

void OS::exit ()
{
    // Lock the mutex first
    std::lock_guard<std::mutex> lock(this->mutex);

    this->running = false;

    if (this->thread.joinable()) {
        this->thread.join();
    }
}
