#include <algorithm> // for std::clamp if using C++17 TBF, i can just use if else
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <limits>

#include "../inc/process.h"

void Process::execute (DisplayManager &dm)
{
    // Maintain a reference to the instruction pointer (don't claim ownership)
    std::shared_ptr<Instruction> &instr = instructions[this->programCounter];

    // Execute a different method depending on instruction ID
    switch (instr->id) {
    case PRINT:
        _PRINT(instr->args);
        break;
    case DECLARE:
        _DECLARE(instr->args);
        break;
    case ADD:
        _ADD(instr->args);
        break;
    case SUBTRACT:
        _SUBTRACT(instr->args);
        break;
    case SLEEP:
        _SLEEP(instr->args);
        break;
    case FOR:
        _FOR(instr->args);
        break;
    default:
        break;
    }

    this->programCounter++;

    // If no instructions left in queue, set status to TERMINATED
    if (this->programCounter >= this->instructions.size()) {
        this->setState(TERMINATED);
    }
}

uint16_t Process::getArgValueUINT16 (std::any &arg)
{
    if (arg.type() == typeid(std::string)) {
        return this->variables[std::any_cast<std::string>(arg)];
    } else {
        return std::any_cast<uint16_t>(arg);
    }
};

void Process::_PRINT (std::vector<std::any> &args)
{
    std::time_t now = std::time(0);

    print_stream
        // Log timestamp
        << std::put_time(std::localtime(&now), "(%m/%d/%Y %H:%M:%S) ")
        // Core information
        << "Core: " << this->core <<
        // Message
        " \"" << std::any_cast<std::string>(args[0]) << "\"\n";
}

void Process::_DECLARE (std::vector<std::any> &args)
{
    // DECLARE (var, uint16)
    variables[std::any_cast<std::string>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]));
}

void Process::_ADD (std::vector<std::any> &args)
{
    // ADD (var, var/uint16, var/uint16)
    int result = getArgValueUINT16(args[1]) + getArgValueUINT16(args[2]);

    // Clamp uint16 to [0, max(uint16)]
    result = std::clamp(result, 0,
                        static_cast<int>(std::numeric_limits<uint16_t>::max()));

    // Save result to var
    variables[std::any_cast<std::string>(args[0])] =
        static_cast<uint16_t>(result);
}

void Process::_SUBTRACT (std::vector<std::any> &args)
{
    // SUBTRACT (var, var/uint16, var/uint16)
    int result = getArgValueUINT16(args[1]) - getArgValueUINT16(args[2]);

    // Clamp uint16 to [0, max(uint16)]
    result = std::clamp(result, 0,
                        static_cast<int>(std::numeric_limits<uint16_t>::max()));

    // Save result to var
    variables[std::any_cast<std::string>(args[0])] =
        static_cast<uint16_t>(result);
}

void Process::_SLEEP (std::vector<std::any> &args)
{
    // Set to WAITING so that processs gets moved to sleep queue in next cycle
    this->state = WAITING;
    this->remainingWaitingCycles = getArgValueUINT16(args[0]);
}

// TODO: This
void Process::_FOR (std::vector<std::any> &args) {
    for (int i = 0; i < getArgValueUINT16(args[1]); i++) {
        for (Instruction inst : args[0]) {
            instructions.insert(instructions.begin() + this->programCounter + 1, inst)
        }
    }

}

std::shared_ptr<Instruction> Process::createInstruction (int depth)
{
    // Create a new Instruction and establish a shareable pointer to it
    std::shared_ptr<Instruction> instruction(new Instruction);

    // Avoid generating a FOR instruction beyond depth = 3
    // ...when we finish fixing it, for now it avoids FOR completely
    instruction->id =
        static_cast<InstructionID>(rand() % (depth > 0 && depth < 3 ? 5 : 5));
    //                     TODO: Set this to 6 to enable FOR instructions ^

    switch (instruction->id) {
    case PRINT: {
        // TODO: Wtf does "message should be able to store a variable" mean
        instruction->args = {"Hello world from " + this->name + "!"};
        break;
    }

    case DECLARE: {
        std::string var = generateVariableName(1);
        uint16_t val = rand() % 65536;

        // var, uint16
        instruction->args = {var, val};
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

        // var, var/uint16, var/uint16
        instruction->args = {var1, var2, var3};

        break;
    }

    case SLEEP: {
        // uint16
        instruction->args = {(uint16_t)(rand() % 256)}; // uint16_t
        break;
    }

    case FOR: {
        int n = rand() % 256 + 1; // Random argument in range [1, 256]

        /* TODO: FOR takes in` a set of instruction, so query create n times
        std::vector<Instruction> instructions;
        int random_lines = rand() % 256 + 1;
        for (int i = 0; i < random_lines; i++) {
            this->instructions.push_back(
                createInstruction(rand() % 2 == 0 ? depth + 1 : 0));
        }
        */

        instruction->args = {createInstruction(depth + 1), n};
        break;
    }
    }

    return std::move(instruction);
}
