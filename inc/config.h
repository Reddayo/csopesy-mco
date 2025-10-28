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

    void setNumCPU(uint32_t numCPU);

    void setScheduler(std::string scheduler);

    void setQuantumCycles(uint32_t quantumCycles);

    void setBatchProcessFreq(uint32_t batchProcessFreq);

    void setMinIns(uint32_t minIns);

    void setMaxIns(uint32_t maxIns);

    void setDelayPerExec(uint32_t delaysPerExec);

    // Getters

    uint32_t getNumCPU();

    SchedulingAlgorithm getScheduler();

    uint32_t getQuantumCycles();

    uint32_t getBatchProcessFreq();

    uint32_t getMinIns();

    uint32_t getMaxIns();

    uint32_t getDelaysPerExec();

  private:
    uint32_t numCPU;

    SchedulingAlgorithm scheduler;

    uint32_t quantumCycles;

    uint32_t batchProcessFreq;

    uint32_t minIns;

    uint32_t maxIns;

    uint32_t delaysPerExec;
};

#endif
