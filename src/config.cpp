#include <fstream>
#include <stdexcept>
#include <string>

#include "../inc/config.h"
#include "../inc/helper.h"

// Setters

void Config::setNumCPU (uint32_t numCPU)
{
    if (numCPU < 1 || numCPU > 128)
        throw std::runtime_error("Number of cores must be within [1, 128]");

    this->numCPU = numCPU;
}

void Config::setScheduler (std::string scheduler)
{
    if (scheduler == "\"fcfs\"") {
        this->scheduler = FCFS;
    } else if (scheduler == "\"rr\"") {
        this->scheduler = RR;
    } else {
        throw std::runtime_error("Invalid scheduling algorithm");
    }
}

void Config::setQuantumCycles (uint32_t quantumCycles)
{
    if (quantumCycles == 0)
        throw std::runtime_error("Time quantum must be positive");

    this->quantumCycles = quantumCycles;
}

void Config::setBatchProcessFreq (uint32_t batchProcessFreq)
{
    if (batchProcessFreq == 0)
        throw std::runtime_error("Batch process frequency must be positive");

    this->batchProcessFreq = batchProcessFreq;
}

void Config::setMinIns (uint32_t minIns)
{
    if (minIns == 0)
        throw std::runtime_error("Minimum instructions must be positive");

    this->minIns = minIns;
}

void Config::setMaxIns (uint32_t maxIns)
{
    if (maxIns == 0)
        throw std::runtime_error("Maximum instructions must be positive");

    this->maxIns = maxIns;
}

void Config::setDelayPerExec (uint32_t delaysPerExec)
{
    this->delaysPerExec = delaysPerExec;
}

void Config::setMaxOverAllMem(uint32_t maxOverAllMem){

    if (maxOverAllMem == 0)
        throw std::runtime_error("Maximum instructions must be positive");

    if (isPowerOf2NInRange(maxOverAllMem))
        throw std::runtime_error("Memory is not power of 2 and in range");

    this->maxOverAllMem = maxOverAllMem;
}

void Config::setMemPerFrame(uint32_t memPerFrame){

    if (memPerFrame == 0)
        throw std::runtime_error("Memory instructions must be positive");
    
    if (isPowerOf2NInRange(memPerFrame))
        throw std::runtime_error("Memory is not power of 2 and in range");

    
    this->memPerFrame = memPerFrame;
}

void Config::setMinMemPerProc(uint32_t minMemPerProc){

    if (minMemPerProc == 0)
        throw std::runtime_error("Minimum instructions must be positive");

    if (isPowerOf2NInRange(minMemPerProc))
        throw std::runtime_error("Memory is not power of 2 and in range");

    
    this->minMemPerProc = minMemPerProc;
}

void Config::setMaxMemPerProc(uint32_t maxMemPerProc){

    if (maxMemPerProc == 0)
        throw std::runtime_error("Maximum instructions must be positive");

    if (isPowerOf2NInRange(maxMemPerProc))
        throw std::runtime_error("Memory is not power of 2 and in range");

    this->maxMemPerProc = maxMemPerProc;
}

// Getters

uint32_t Config::getNumCPU () { return this->numCPU; }

enum SchedulingAlgorithm Config::getScheduler () { return this->scheduler; }

uint32_t Config::getQuantumCycles () { return this->quantumCycles; }

uint32_t Config::getBatchProcessFreq () { return this->batchProcessFreq; }

uint32_t Config::getMinIns () { return this->minIns; }

uint32_t Config::getMaxIns () { return this->maxIns; }

uint32_t Config::getDelaysPerExec () { return this->delaysPerExec; }

uint32_t Config::getMaxOverAllMem(){ return this->maxOverAllMem;  }

uint32_t Config::getMemPerFrame(){ return this->memPerFrame; }

uint32_t Config::getMinMemPerProc(){ return this->minMemPerProc; }

uint32_t Config::getMaxMemPerProc(){ return this->maxMemPerProc; }

uint32_t Config::getNumFrames(){ return this->maxOverAllMem / this->memPerFrame; }
// Constructor

Config::Config (std::string filename)
{
    // Read file as stream
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string key, value;

        // Parse config key
        while (file >> key) {
            file >> value;

            if (key == "num-cpu") {
                this->setNumCPU(std::stoul(value));
            } else if (key == "scheduler") {
                this->setScheduler(value);
            } else if (key == "quantum-cycles") {
                this->setQuantumCycles(std::stoul(value));
            } else if (key == "batch-process-freq") {
                this->setBatchProcessFreq(std::stoul(value));
            } else if (key == "min-ins") {
                this->setMinIns(std::stoul(value));
            } else if (key == "max-ins") {
                this->setMaxIns(std::stoul(value));
            } else if (key == "delay-per-exec") {
                this->setDelayPerExec(std::stoul(value));
            } else if (key == "max-overall-mem"){
                this->setMaxOverAllMem(std::stoul(value));
            } else if (key == "mem-per-frame"){
                this->setMemPerFrame(std::stoul(value));
            } else if (key == "min-mem-per-proc"){
                this->setMinMemPerProc(std::stoul(value));
            } else if (key == "max-mem-per-proc"){
                this->setMaxMemPerProc(std::stoul(value));
            }  else {
                throw std::runtime_error(key);
            }
        }
    } else {
        throw std::runtime_error("Unable to open config.txt file");
    }
}
