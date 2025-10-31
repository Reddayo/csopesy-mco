#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <memory>
#include <queue>
#include <vector>

#include "core.h"
#include "process.h"

enum SchedulingAlgorithm { FCFS, RR };

class Scheduler
{
  public:
    Scheduler(enum SchedulingAlgorithm scheduler_type, uint32_t quantum);

    /**
     * Pops a process from the ready queue, then dispatches it to a core.
     *
     * @param core  The core that will be assigned the process
     */
    void dispatch(Core &core);

    // void setQuantum(int timeQuantum);

    uint32_t getQuantum();

    /**
     * Adds a process to the ready queue. Accepts a reference to a shared
     * pointer to the Process and transfers ownership of it to the queue.
     *
     * @param process A reference to a shared pointer to a Process
     */
    void addProcess(std::shared_ptr<Process> &process);

    /**
     * Adds a process to the sleep queue. Accepts a reference to a shared
     * pointer to the Process and transfers ownership of it to the queue.
     *
     * @param process A reference to a shared pointer to a Process
     */
    void sleepProcess(std::shared_ptr<Process> &process);

    bool isQueueEmpty();

    /**
     * Decrements waiting queue for all sleeping processes by 1.
     */
    void countDownSleepingProcesses();

  private:
    /** Scheduling algorithm */
    enum SchedulingAlgorithm algorithm;

    /** Ready queue containing shared pointers to Processes */
    std::queue<std::shared_ptr<Process>> readyQueue;

    std::vector<std::shared_ptr<Process>> sleepQueue;

    /** Time quantum for RR algorithm, ignored in FCFS */
    uint32_t timeQuantumRR;
};

#endif
