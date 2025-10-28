#include "../inc/scheduler.h"

Scheduler::Scheduler (std::string scheduler_type, int quantum)
{
    /* Not sure what else is needed in scheduler */
    this.scheduler_type = scheduler_type;
    this.quantum = quantum;
}
