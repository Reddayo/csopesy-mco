#include <fstream>
#include <stdexcept>
#include <string>

#include "../inc/config.h"

// Setters

void Config::setNumCPU (unsigned long int numCPU)
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

void Config::setQuantumCycles (unsigned long int quantumCycles)
{
    if (quantumCycles == 0)
        throw std::runtime_error("Time quantum must be positive");

    this->quantumCycles = quantumCycles;
}

void Config::setBatchProcessFreq (unsigned long int batchProcessFreq)
{
    if (batchProcessFreq == 0)
        throw std::runtime_error("Batch process frequency must be positive");

    this->batchProcessFreq = batchProcessFreq;
}

void Config::setMinIns (unsigned long int minIns)
{
    if (minIns == 0)
        throw std::runtime_error("Minimum instructions must be positive");

    this->minIns = minIns;
}

void Config::setMaxIns (unsigned long int maxIns)
{
    if (maxIns == 0)
        throw std::runtime_error("Maximum instructions must be positive");

    this->maxIns = maxIns;
}

void Config::setDelayPerExec (unsigned long int delaysPerExec)
{
    this->delaysPerExec = delaysPerExec;
}

// Getters

unsigned long int Config::getNumCPU () { return this->numCPU; }

enum SchedulingAlgorithm Config::getScheduler () { return this->scheduler; }

unsigned long int Config::getQuantumCycles () { return this->quantumCycles; }

unsigned long int Config::getBatchProcessFreq ()
{
    return this->batchProcessFreq;
}

unsigned long int Config::getMinIns () { return this->minIns; }

unsigned long int Config::getMaxIns () { return this->maxIns; }

unsigned long int Config::getDelaysPerExec () { return this->delaysPerExec; }

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
            } else {
                throw std::runtime_error(key);
            }
        }
    } else {
        throw std::runtime_error("Unable to open config.txt file");
    }
}
