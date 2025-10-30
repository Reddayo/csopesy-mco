#include <list>
#include <mutex>
#include <string>
#include <thread>

#include "../inc/core.h"
#include "../inc/os.h"
#include "../inc/scheduler.h"

void OS::run ()
{
    this->running = true;

    this->thread = std::thread([this] () {
        int processAutoId = 0;

        this->resetCycles();

        while (this->running) {
            std::lock_guard<std::mutex> lock(this->mutex);

            // Create a new process and add it to the scheduler's ready queue
            // every n cycles (dictated by batch-process-freq)
            if (this->generateDummyProcesses &&
                this->cycle % this->config.getBatchProcessFreq() == 0) {

                // Create a new process and wrap it in a unique pointer
                std::unique_ptr<Process> process(new Process(
                    processAutoId, "process" + std::to_string(processAutoId),
                    this->config.getMinIns() +
                        rand() % (this->config.getMaxIns() -
                                  this->config.getMinIns() + 1)));

                // Transfer ownership of the unique pointer to the scheduler
                // queue This calls std::move() internally
                this->scheduler.addProcess(process);
                processAutoId++;
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

                                    this->finishedProcesses.push_back(
                                        std::pair<int, int>(
                                            process->getId(),
                                            process->getProgramCounter()));

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
