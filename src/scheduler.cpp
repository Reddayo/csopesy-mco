#include "../inc/scheduler.h"
#include <cstdint>

Scheduler::Scheduler (enum SchedulingAlgorithm scheduler_type, uint32_t quantum)
    : algorithm(scheduler_type), timeQuantumRR(quantum)
{
    // Additional logic here, unsure what though
}

void Scheduler::dispatch (Core &core)
{
    std::unique_ptr<Process> &process = readyQueue.front();
    process->setState(RUNNING);
    process->setLastCoreID(core.getId());

    // Ownership of unique pointer is transferred to Core
    core.setProcess(process);

    // readyQueue.front() is nulled by this point; pop to remove
    readyQueue.pop();
}

uint32_t Scheduler::getQuantum () { return this->timeQuantumRR; }

void Scheduler::addProcess (std::unique_ptr<Process> &process)
{
    readyQueue.push(std::move(process));
}

bool Scheduler::isQueueEmpty () { return this->readyQueue.empty(); }
