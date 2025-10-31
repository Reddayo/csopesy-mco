#include "../inc/scheduler.h"
#include <cstdint>
#include <stdexcept>

Scheduler::Scheduler (enum SchedulingAlgorithm scheduler_type, uint32_t quantum)
    : algorithm(scheduler_type), timeQuantumRR(quantum)
{
    // Additional logic here, unsure what though
}

void Scheduler::dispatch (Core &core)
{
    std::shared_ptr<Process> &process = readyQueue.front();
    process->setState(RUNNING);
    process->setLastCoreID(core.getId());

    // Ownership of shared pointer is transferred to Core
    core.setProcess(process);

    // readyQueue.front() is nulled by this point; pop to remove
    readyQueue.pop_front();
}

uint32_t Scheduler::getQuantum () { return this->timeQuantumRR; }

void Scheduler::addProcess (std::shared_ptr<Process> &process)
{
    readyQueue.push_back(std::move(process));
}

void Scheduler::sleepProcess (std::shared_ptr<Process> &process)
{
    sleepQueue.push_back(std::move(process));
}

bool Scheduler::isQueueEmpty () { return this->readyQueue.empty(); }

std::deque<std::shared_ptr<Process>> &Scheduler::getReadyQueue ()
{
    return this->readyQueue;
}

std::vector<std::shared_ptr<Process>> &Scheduler::getSleepQueue ()
{
    return this->sleepQueue;
}

void Scheduler::countDownSleepingProcesses ()
{
    for (auto it = sleepQueue.begin(); it != sleepQueue.end();) {
        // TODO: Not sure exactly why this is necessary but shouganai
        if ((*it) == nullptr) {
            it = sleepQueue.erase(it);
            continue;
        }

        // Decrement sleep timer by 1
        (*it)->decrementWaitingCycles();

        // Return to ready queue if sleep timer is 0
        if ((*it)->getRemainingWaitingCycles() == 0) {
            (*it)->setState(READY);
            this->addProcess((*it));
            it = sleepQueue.erase(it);
        }
    }
}
