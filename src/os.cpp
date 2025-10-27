#include "../inc/os.h"
#include "../inc/scheduler.h"

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
