#ifndef OS_H
#define OS_H

#include "scheduler.h"

class OS
{
  public:
    OS(int numCores,                       // Number of cores to create
       enum SchedulingAlgorithm algorithm, // Scheduling algorithm to use
       int timeQuantumRR,                  // Time quantum for RR
       int batchProcessFreq,               // Generate processes every n cycles
       int minIns,                         // Minimum # of instructions
       int maxIns,                         // Maximum # of instructions
       int delay                           // Delay after instruction execution
    );

    /** OS main loop */
    void run();
};

#endif
