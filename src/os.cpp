#include "../inc/os.h"
#include "../inc/scheduler.h"
#include <list>

// WARNING: None of this works yet

OS::OS(DisplayManager &dm, Config &config)
    : dm(dm),        // Display manager
      config(config) // Config values
// scheduler(Scheduler(config.getScheduler())) // Scheduler
{};

void OS::run ()
{
    /*
        this->resetCycles();

        int batchProcessFreq = this->config.getBatchProcessFreq();

        while (true) {
            // Create a new process and add it to the scheduler's ready queue
            if (cycle % batchProcessFreq == 0) {
                this->scheduler.addProcess(Process());
            }

            // Pre-empt processes if RR algorithm
        }
    */
}

void OS::ls ()
{
    std::string status_string;

    // Number of cores
    int nCores = this->cores.size();

    // Running cores
    std::list<int> runningCoreIds;
    for(int i = 0; i < nCores; i++) {
        if(cores[i].isRunning())
            runningCoreIds.push_back(cores[i].getId());
    }

    // Number of running cores
    int nRunningCores = runningCoreIds.size();
    
    // CPU utilization
    float cpuUtil = nRunningCores / nCores * 100;

    // Status string
    status_string = "CPU utilization: " + std::to_string(cpuUtil) + "\%\n" +
                    "Cores used: " + std::to_string(nRunningCores) + "\n" +
                    "Cores available: " + std::to_string(nCores - nRunningCores) + "\n\n";
    
    // Running processes
    status_string += "Running processes:\n";
    for(int i : runningCoreIds) {
        // Process name is same as ID .....I think
        status_string += "process ";
        status_string += std::to_string(cores[i].getProcess().getId());

        // Core name is same as ID ............I think
        status_string += "      Core ";
        status_string += std::to_string(cores[i].getId());

        status_string += "     ";
        // Progress
        status_string += std::to_string(cores[i].getProcess().getNumCycles());
        status_string += " / ";
        status_string += std::to_string(cores[i].getProcess().getNumFinCycles());

        status_string += "\n";
    }

    // Finished processes
    status_string += "\n";
    status_string += "Finished processes:\n";

    for(std::pair<int, int> finishedProcess : this->finishedProcesses) {
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
    this->dm._mvwprintw(0, 0, "%s", status_string);
}