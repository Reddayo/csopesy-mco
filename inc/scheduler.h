#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>

#include "core.h"
#include "process.h"

enum SchedulingAlgorithm { FCFS, RR };

class Scheduler
{
  public:
    Scheduler(enum SchedulingAlgorithm algorithm);

    /**
     * Pops a process from the ready queue and executes it, depending on the
     * scheduling algorithm
     */
    void dispatch(Core &core);

    void setQuantum(int timeQuantum);

    int getQuantum();

    /** Adds a process to the ready queue */
    void addProcess(Process process);

    bool isQueueEmpty();

  private:
    /** Scheduling algorithm */
    enum SchedulingAlgorithm algorithm;

    /** Ready queue containing processes */
    std::queue<Process> readyQueue;

    /** Time quantum for RR algorithm, ignored in FCFS */
    int timeQuantumRR;
};

#endif
