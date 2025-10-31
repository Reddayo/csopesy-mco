#include <cstdint>
#include <ctime>
#include <string>

#include "../inc/process.h"

// ARE YOU RE@DY!! I'M L@DY!! HAJIMEYOU YAREBA DEKIRU KITTO ZETTAI WATASHI #1
Process::Process (int id, std::string name, uint32_t instruction_count)
    : id(id), name(name), state(READY), programCounter(0)
{
    randomizeInstructions(instruction_count);
    this->startTime = std::time(0);
}

std::string Process::getName () { return this->name; }

int Process::getId () { return this->id; }

std::time_t Process::getStartTime () { return this->startTime; }

void Process::setState (enum ProcessState state) { this->state = state; }

enum ProcessState Process::getState () { return this->state; }

uint32_t Process::getProgramCounter () { return this->programCounter; }

// TODO: Deal with instruction size logic here
uint32_t Process::getTotalCycles ()
{
    /** Return number of cycles */
    return this->instructions.size();
}

// Elapsed cycles

uint32_t Process::getElapsedCycles () { return this->elapsedCycles; }

void Process::incrementElapsedCycles () { elapsedCycles++; }

void Process::resetElapsedCycles () { this->elapsedCycles = 0; }

// Busy-waiting cycles

uint32_t Process::getRemainingBusyWaitingCycles ()
{
    return this->remainingBusyWaitingCycles;
}

void Process::setBusyWaitingCycles (uint32_t value)
{
    this->remainingBusyWaitingCycles = value;
}

void Process::decrementBusyWaitingCycles ()
{
    this->remainingBusyWaitingCycles--;
}

// Waiting cycles

// TODO: Data type
uint16_t Process::getRemainingWaitingCycles ()
{
    return this->remainingWaitingCycles;
}

void Process::setWaitingCycles (uint16_t value)
{
    this->remainingWaitingCycles = value;
}

void Process::decrementWaitingCycles () { this->remainingWaitingCycles--; }

std::string Process::getStateAsString ()
{
    std::stringstream ss;

    ss << "Process name: " << this->name << "\n";
    ss << "Process ID: " << this->id << "\n\n";
    ss << "Current instruction line: " << this->programCounter << "\n";
    ss << "Total instructions: " << this->instructions.size() << "\n\n";
    ss << "Logs:\n" << this->print_stream.str();

    return ss.str();
}

void Process::setLastCoreID (uint32_t core) { this->core = core; }

// TODO: Depending on how sir responds, this might be replaced with a
// randomizeCommands + assembleCommands or something to that effect
void Process::randomizeInstructions (int instruction_count)
{
    // NOTE: Count is randomly assigned by the scheduler
    for (int i = 0; i < instruction_count; i++) {
        this->instructions.push_back(createInstruction());
    }
}

std::string Process::generateVariableName (bool uniqueness)
{
    // WARNING: Uniqueness is not enforced by default
    std::string varName;

    do {
        varName =
            std::string(1, 'a' + (rand() % 26)) + std::to_string(rand() % 1000);
    }
    // Randomly generate names until you get a unique one
    while (uniqueness == true && variables.count(varName));

    return varName;
}
