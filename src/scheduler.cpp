#include "../inc/scheduler.h"

/** Not Done.. idk if this is right */
void Scheduler::dispatch (Core &core)
{

    /** If core is available, pop a process, or is this handled by the os */
    // if(!core.isRunning() && !readyQueue.empty()){
    Process process = readyQueue.front();
    readyQueue.pop();
    core.setProcess(process);
    // core.running = true;
    //}
}

void Scheduler::addProcess (Process process)
{

    /** adds a process to readyqueue */
    readyQueue.push(process);
}
