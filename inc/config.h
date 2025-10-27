#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <variant>

#include "scheduler.h"

class Config
{
  public:
    Config(std::string filename);

    // Setters

    void setNumCPU(unsigned long int numCPU);

    void setScheduler(std::string scheduler);

    void setQuantumCycles(unsigned long int quantumCycles);

    void setBatchProcessFreq(unsigned long int batchProcessFreq);

    void setMinIns(unsigned long int minIns);

    void setMaxIns(unsigned long int maxIns);

    void setDelayPerExec(unsigned long int delaysPerExec);

    // Getters

    unsigned long int getNumCPU();

    SchedulingAlgorithm getScheduler();

    unsigned long int getQuantumCycles();

    unsigned long int getBatchProcessFreq();

    unsigned long int getMinIns();

    unsigned long int getMaxIns();

    unsigned long int getDelaysPerExec();

  private:
    unsigned long int numCPU;

    SchedulingAlgorithm scheduler;

    unsigned long int quantumCycles;

    unsigned long int batchProcessFreq;

    unsigned long int minIns;

    unsigned long int maxIns;

    unsigned long int delaysPerExec;
};

#endif
