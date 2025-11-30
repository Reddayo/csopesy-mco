#include <cstdint>
#include <ctime>
#include <string>

#include "../inc/process.h"

// ARE YOU RE@DY!! I'M L@DY!! HAJIMEYOU YAREBA DEKIRU KITTO ZETTAI WATASHI #1 //
// TODO: might be bad
Process::Process (int id,
                  std::string name,
                  uint32_t instruction_count,
                  uint32_t memory_size,
                  uint32_t mem_per_frame)
    : id(id), name(name), state(READY), programCounter(0),
      memory_size(memory_size), mem_per_frame(mem_per_frame)
{
    requiredPages = (memory_size + mem_per_frame - 1) / mem_per_frame;
    this->print_stream << "Required Pages: " << requiredPages
                       << "; Memory Size: " << memory_size << "\n";
    // this->variables["x"] = 0; //im assuming this doesn't really do anything
    /* this->memsize = memsize; */

    this->startTime = std::time(0);
    this->logicalAddressCounter = 0;
    this->memorySize = memory_size;
    randomizeInstructions(instruction_count);
}

std::string Process::getName () { return this->name; }

int Process::getId () { return this->id; }

std::time_t Process::getStartTime () { return this->startTime; }

void Process::setState (enum ProcessState state) { this->state = state; }

enum ProcessState Process::getState () { return this->state; }

uint32_t Process::getProgramCounter () { return this->programCounter; }

uint32_t Process::getTotalCycles ()
{
    /** Return number of cycles */
    return this->totalCycles;
}

// Elapsed ---------------------------------------------------------------------

uint32_t Process::getElapsedCycles () { return this->elapsedCycles; }

void Process::incrementElapsedCycles () { elapsedCycles++; }

void Process::resetElapsedCycles () { this->elapsedCycles = 0; }

// Busy-waiting cycles ---------------------------------------------------------

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

// Waiting cycles --------------------------------------------------------------

// TODO: Data type
uint16_t Process::getRemainingWaitingCycles ()
{
    return this->remainingWaitingCycles;
}

void Process::setWaitingCycles (uint16_t value)
{
    this->remainingWaitingCycles = value;
}

void Process::setTotalCycles (int totalCycles)
{
    this->totalCycles = totalCycles;
}

void Process::decrementWaitingCycles () { this->remainingWaitingCycles--; }

// -----------------------------------------------------------------------------

std::string Process::getStateAsString ()
{
    std::stringstream ss;

    ss << "Process name: " << this->name << "\n";
    ss << "Process ID: " << this->id << "\n\n";
    ss << "Current instruction line: " << this->programCounter << "\n";
    ss << "Total instructions: " << this->getTotalCycles() << "\n\n";
    ss << "Logs:\n" << this->print_stream.str();

    return ss.str();
}

void Process::setLastCoreID (uint32_t core) { this->core = core; }

void Process::randomizeInstructions (int instruction_count)
{
    // NOTE: Count is randomly assigned by the scheduler
    int instCtr = 0;
    while (instCtr < instruction_count) {
        this->instructions.push_back(
            std::move(createInstruction(0, &instCtr, instruction_count, 1)));
    }

    this->setTotalCycles(instCtr);
}

std::string Process::generateVariableName (bool uniqueness)
{
    // WARNING: Uniqueness is not enforced by default
    std::string varName;

    // 50/50 reuse old or generate new
    if (uniqueness || variables.empty()) {
        do {
            varName = "var" + std::to_string(variableCounter++);
        } while (variables.count(varName));
    } else {

        bool roll = (rand() % 2 == 0);

        if (roll) {
            varName = "var" + std::to_string(variableCounter++);
        } else {
            int index = rand() % variables.size();
            auto it = variables.begin();
            std::advance(it, index);
            varName = it->first;
        }
    }

    return varName;
}
