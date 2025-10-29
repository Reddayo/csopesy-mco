#include <list>
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
    this->thread = std::thread([this] () {
        int processId = 0;

        this->resetCycles();

        while (true) {

            // Create a new process and add it to the scheduler's ready queue
            // every n cycles (dictated by batch-process-freq)
            if (this->cycle % this->config.getBatchProcessFreq() == 0 &&
                // Maximum 500 processes
                processId < 500) {
                // TODO: Create Process on heap with new operator maybe?
                Process process = Process(processId, this->config.getMaxIns());

                this->scheduler.addProcess(process);
                processId++;
            }

            // TODO: Pre-empt processes if RR algorithm
            // Pre-empting means setting status to WAITING and then setting
            // core.running to false

            // Dispatch processes to any available cores
            for (Core &core : this->cores) {
                if (!(this->scheduler.isQueueEmpty())) {
                    if (!core.isRunning()) {
                        this->scheduler.dispatch(core);

                        // When a core gets a process, its state becomes RUNNING
                        core.setRunning(true);
                    }
                }
            }

            // Execute each core's process in a separate thread
            for (Core &core : this->cores) {
                if (core.isRunning()) {
                    std::thread thread = std::thread([this, &core] () {
                        Process &process = core.getProcess();

                        // Check if process is running first
                        if (process.getState() == RUNNING) {

                            // If process is NOT in a busy waiting state...
                            if (process.getRemainingBusyWaitingCycles() == 0) {
                                process.execute(dm);

                                // When process terminates, core stops running
                                if (process.getState() == TERMINATED) {
                                    // TODO: Move to terminated processes?
                                    core.setRunning(false);
                                } else {
                                    // Otherwise, delay execution
                                    process.setBusyWaitingCycles(
                                        this->config.getDelaysPerExec());
                                    process.incrementElapsedCycles();
                                }

                            } else {
                                // Delay execution until busy-waiting timer out
                                process.decrementBusyWaitingCycles();
                                process.incrementElapsedCycles();
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

void OS::ls ()
{
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
        status_string += std::to_string(cores[i].getProcess().getId());

        // Core name is same as ID ............I think
        status_string += "      Core ";
        status_string += std::to_string(cores[i].getId());

        status_string += "     ";
        // Progress
        status_string +=
            std::to_string(cores[i].getProcess().getProgramCounter());
        status_string += " / ";
        status_string += std::to_string(cores[i].getProcess().getTotalCycles());

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
