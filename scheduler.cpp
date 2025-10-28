#include "../inc/scheduler.h"

enum SchedulingAlgorithm { FCFS, RR };

Scheduler::Scheduler (SchedulingAlgorithm algorithm, int timeQuantumRR)
{
    std::queue<Process> readyQueue;

    this.algorithm = algorithm;
    this.timeQuantumRR = timeQuantumRR;
}
