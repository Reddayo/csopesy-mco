#include "../inc/process.h"
#include <cstdint>
#include <string>
<<<<<<< HEAD
#include <ctime>
    == == ==
    =

>>>>>>> print

        Process::~Process()
{
    for (Instruction inst : this->instructions) inst.args.clear();
    this->instructions.clear();
    this->variables.clear();
    this->startTime = std::time(0);
}

// ARE YOU RE@DY!! I'M L@DY!! HAJIMEYOU YAREBA DEKIRU KITTO ZETTAI WATASHI #1
Process::Process (std::string name, uint32_t instruction_count)
    : name(name), state(READY), programCounter(0)
{
    randomizeInstructions(instruction_count);
    this->startTime = std::time(0);
}

std::string Process::getName () { return this->name; }

int Process::getId () { return this->id; }

std::time_t Process::getStartTime () { return this->startTime; }

void Process::setState (enum ProcessState state) { this->state = state; }

enum ProcessState Process::getState () { return this->state; }

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
    return this->elapsedBusyWaitingCycles;
}

void Process::setBusyWaitingCycles (uint32_t value)
{
    this->elapsedBusyWaitingCycles = value;
}

void Process::decrementBusyWaitingCycles ()
{
    this->elapsedBusyWaitingCycles--;
}

// Waiting cycles

// TODO: Data type
uint16_t Process::getRemainingWaitingCycles ()
{
    return this->elapsedWaitingCycles;
}

void Process::setWaitingCycles (uint16_t value)
{
    this->elapsedWaitingCycles = value;
}

void Process::decrementWaitingCycles () { this->elapsedWaitingCycles--; }

uint32_t Process::getProgramCounter () { return this->programCounter; }

// TODO: Depending on how sir responds, this might be replaced with a
// randomizeCommands + assembleCommands or something to that effect
void Process::randomizeInstructions (int instruction_count)
{
    // NOTE: Count is randomly assigned by the scheduler
    for (int i = 0; i < instruction_count; i++) {
        this->instructions.push_back(createInstruction());
    }
}

Instruction Process::createInstruction (int depth)
{
    Instruction instruction;

    // Avoid generating a FOR instruction beyond depth = 3
    // ...when we finish fixing it, for now it avoids FOR completely
    instruction.id =
        static_cast<InstructionID>(rand() % (depth > 0 && depth < 3 ? 5 : 5));
    //                     TODO: Set this to 6 to enable FOR instructions ^

    switch (instruction.id) {

    case PRINT: {
        instruction.args.push_back("Hello world from process" +
                                   std::to_string(this->id) + "!");
        break;
    }

    case DECLARE: {
        std::string var = generateVariableName(1);
        instruction.args.push_back(var);

        uint16_t val = rand() % 65536;
        instruction.args.push_back(val);

        break;
    }

    case ADD:
    case SUBTRACT: {
        // First argument is always a destination variable
        std::string var1 = generateVariableName();

        // Other arguments can be a random variable or a random uint16_t value
        // TODO: Move to actual implementation of instructions instead of here
        auto createSource = [this] () -> std::any {
            if (rand() % 2 == 0) {
                return (uint16_t)(rand() % 65536);
            } else {
                return generateVariableName();
            }
        };

        std::any var2 = createSource();
        std::any var3 = createSource();

        instruction.args = {var1, var2, var3};
        break;
    }

    case SLEEP: {
        instruction.args.push_back(rand() % 256); // uint8
        break;
    }

    case FOR: {
        int n = rand() % 256 + 1; // Random argument in range [1, 256]
        //  TODO: FOR takes in` a set of instruction, so query create n times
        /*
            std::vector<Instruction> instructions;
            int random_lines = rand() % 256 + 1;
            for (int i = 0; i < random_lines; i++){
                instructions.push_back(createInstruction(rand() % 2 == 0 ? depth
           + 1 : 0));
            }
        */
        instruction.args = {createInstruction(depth + 1), n};
        break;
    }
    }

    return instruction;
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
