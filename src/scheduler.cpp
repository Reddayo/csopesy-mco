#include "../inc/scheduler.h"
#include <cstdint>

Scheduler::Scheduler (enum SchedulingAlgorithm scheduler_type, uint32_t quantum)
    : algorithm(scheduler_type), timeQuantumRR(quantum)
{
    // Additional logic here, unsure what though
}

void Scheduler::dispatch (Core &core)
{
    Process process = readyQueue.front();
    process.setState(RUNNING);
    core.setProcess(process);
    readyQueue.pop();
}

uint32_t Scheduler::getQuantum () { return this->timeQuantumRR; }

void Scheduler::addProcess (Process process) { readyQueue.push(process); }

bool Scheduler::isQueueEmpty () { return this->readyQueue.empty(); }
