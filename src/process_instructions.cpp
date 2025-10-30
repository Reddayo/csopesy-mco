#include <algorithm> // for std::clamp if using C++17 TBF, i can just use if else
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <limits>

#include "../inc/process.h"

// TODO: If no instructions left in queue, set status to TERMINATED
void Process::execute (DisplayManager &dm)
{
    Instruction instr = instructions[this->programCounter];

    switch (instr.id) {

    case PRINT:
        _PRINT(instr.args);
        break;
    case DECLARE:
        _DECLARE(instr.args);
        break;
    case ADD:
        _ADD(instr.args);
        break;
    case SUBTRACT:
        _SUBTRACT(instr.args);
        break;
    case SLEEP:
        _SLEEP(instr.args);
        break;
    case FOR:
        _FOR(instr.args);
        break;
    default:
        break;
    }

    this->programCounter++;

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

// TODO: This
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
    /** _DECLARE var, value
     *   all are uint16
     */
    variables[std::any_cast<std::string>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]));
}

void Process::_ADD (std::vector<std::any> &args)
{
    int result = getArgValueUINT16(args[1]) + getArgValueUINT16(args[2]);
    result = std::clamp(result, 0,
                        static_cast<int>(std::numeric_limits<uint16_t>::max()));
    variables[std::any_cast<std::string>(args[0])] =
        static_cast<uint16_t>(result);
}

void Process::_SUBTRACT (std::vector<std::any> &args)
{
    int result = getArgValueUINT16(args[1]) - getArgValueUINT16(args[2]);
    result = std::clamp(result, 0,
                        static_cast<int>(std::numeric_limits<uint16_t>::max()));
    variables[std::any_cast<std::string>(args[0])] =
        static_cast<uint16_t>(result);
}

void Process::_SLEEP (std::vector<std::any> &args)
{
    this->state = WAITING;
    this->remainingWaitingCycles = getArgValueUINT16(args[0]);
}

// TODO: This
void Process::_FOR (std::vector<std::any> &args) {}

// TODO: Clean this up when we need to
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
        instruction.args.push_back("Hello world from " + this->name + "!");
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
        instruction.args.push_back((uint16_t)(rand() % 256)); // uint16_t
        break;
    }

    case FOR: {
        int n = rand() % 256 + 1; // Random argument in range [1, 256]
        //  TODO: FOR takes in` a set of instruction, so query create n times
        /*
         *           std::vector<Instruction> instructions;
         *           int random_lines = rand() % 256 + 1;
         *           for (int i = 0; i < random_lines; i++){
         *               instructions.push_back(createInstruction(rand() % 2 ==
    0 ? depth
         *          + 1 : 0));
    }
    */
        instruction.args = {createInstruction(depth + 1), n};
        break;
    }
    }

    return instruction;
}
