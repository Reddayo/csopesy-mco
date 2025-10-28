#include "../inc/os.h"
#include "../inc/scheduler.h"

// WARNING: None of this works yet

OS::OS(DisplayManager &dm, Config &config)
    : dm(dm),                                     // Display manager
      config(config),                             // Config values
      scheduler(Scheduler(config.getScheduler())) // Scheduler
{};

void OS::run ()
{

    this->resetCycles();

    int batchProcessFreq = this->config.getBatchProcessFreq();

    while (true) {
        // Create a new process and add it to the scheduler's ready queue
        if (cycle % batchProcessFreq == 0) {
            this->scheduler.addProcess(Process(this->config.getMaxIns()));
        }

        // Pre-empt processes if RR algorithm

        // Dispatch processes to any available cores
        if (!this->scheduler.isQueueEmpty()) {
            for (Core core : this->cores) {
                if (!core.isRunning()) {
                    this->scheduler.dispatch(core);
                }
            }
        }

        // Execute each core's process in a separate thread
        for (Core core : this->cores) {
            if (core.isRunning()) {
                // TODO: Create a new thread here
                core.getProcess().execute();
            }
        }
        // TODO: Join threads here
    }
}
