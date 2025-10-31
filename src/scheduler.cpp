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
    readyQueue.pop_front();
}

uint32_t Scheduler::getQuantum () { return this->timeQuantumRR; }

void Scheduler::addProcess (std::unique_ptr<Process> &process)
{
    readyQueue.push_back(std::move(process));
}

void Scheduler::sleepProcess (std::unique_ptr<Process> &process)
{
    sleepQueue.push_back(std::move(process));
}

bool Scheduler::isQueueEmpty () { return this->readyQueue.empty(); }

std::deque<std::unique_ptr<Process>> &Scheduler::getReadyQueue()
{
    return this->readyQueue;
}

void Scheduler::countDownSleepingProcesses ()
{
    for (auto it = sleepQueue.begin(); it != sleepQueue.end();) {
        // Decrement sleep timer by 1
        (*it)->decrementWaitingCycles();

        // Return to ready queue if sleep timer is 0
        if ((*it)->getRemainingWaitingCycles() == 0) {
            this->addProcess((*it));
            it = sleepQueue.erase(it);
        }
    }
}
