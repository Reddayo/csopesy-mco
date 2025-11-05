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
        this->print_stream << "\nFinished!";
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
        " \"" << std::any_cast<std::string>(args[0]);

    if (args.size() > 1) {
        // Starting at the second argument, treat as variable names
        for (auto i = begin(args) + 1, e = end(args); i != e; ++i) {
            std::string var_name = std::any_cast<std::string>(*i);

            print_stream << this->variables[var_name];
        }
    };

    print_stream << "\"\n";
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
void Process::_FOR (std::vector<std::any> &args)
{
    // I hate this language I'm so sorry for this
    std::vector<std::shared_ptr<Instruction>> &loop =
        std::any_cast<std::vector<std::shared_ptr<Instruction>> &>(args[0]);

    int j = 0;

    for (int i = 0; i < getArgValueUINT16(args[1]); i++) {
        // Make a copy of the shared ptr
        for (std::shared_ptr<Instruction> instruction : loop) {
            this->instructions.insert(
                // Insert at next instruction
                this->instructions.begin() + this->programCounter + 1 + j,
                // Move ownership of the copy to the main instruction queue
                std::move(instruction));
            j++;
        }
    }
}

std::shared_ptr<Instruction> Process::createInstruction (int depth,
                                                         int *instCtr,
                                                         int instruction_count,
                                                         int loopCount)
{
    // Create a new Instruction and establish a shareable pointer to it
    std::shared_ptr<Instruction> instruction(new Instruction);

    // Avoid generating a FOR instruction beyond depth = 3
    // Avoid exceeding instruction count
    // 25 since 5 maximum loops, 5 maximum instructions inside

    instruction->id = static_cast<InstructionID>(
        rand() %
        (depth >= 0 && depth < 3 &&
                 (*instCtr + loopCount + loopCount * 26 < instruction_count)
             ? 6
             : 5));
    //*/

    // TODO: Use this for test case
    // instruction->id = this->instructions.size() % 2 == 0 ? PRINT : ADD;

    // Increment instruction counter
    *instCtr += loopCount;

    switch (instruction->id) {
    case PRINT: {
        instruction->args = {"Hello world from " + this->name + "!"};
        // TODO: Use this one for test case
        // instruction->args = {std::string("Value from: "), std::string("x")};

        /* TODO: Uncomment this if you want to randomly print a "hello world"
           message and a variable *

        if (this->declaredVariableNames.empty() || rand() % 2 == 0) {
            // Print a "hello world" message
            instruction->args = {"Hello world from " + this->name + "!"};
        } else {
            // Pick a random declared variable
            size_t index = rand() % this->declaredVariableNames.size();

            instruction->args = {
                // Message
                "Selected var " + this->declaredVariableNames[index] + " = ",
                // Print a random declared variable
                this->declaredVariableNames[index]};
        }
        //*/

        break;
    }

    case DECLARE: {
        std::string var = generateVariableName(1);
        uint16_t val = rand() % 65536;

        // TODO: Not needed in test cases
        // this->declaredVariableNames.push_back(var);

        // var, uint16
        instruction->args = {var, val};
        break;
    }

    case ADD:
        /* TODO: Uncomment for test case */
        {
            // ADD(x, x, random number in [1, 10])
            instruction->args = {std::string("x"), std::string("x"),
                                 (uint16_t)(rand() % 10 + 1)};
            break;
        } //*/
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
        // Random argument in range [1, 5]
        uint16_t n = rand() % 5 + 1;

        std::vector<std::shared_ptr<Instruction>> instructions;

        // Random number of lines in instruction
        int random_lines = rand() % 5 + 1;
        for (int i = 0; i < random_lines; i++) {

            if (*instCtr + loopCount + loopCount * 26 < instruction_count)
                instructions.push_back(std::move(createInstruction(
                    depth + 1, instCtr, instruction_count, n * loopCount)));
            else
                break;
        }

        // Copies all instructions to the argument, sharing ownership
        instruction->args = {std::move(instructions), n};

        // Instructions vector dies here and its pointers with it

        break;
    }
    }

    return std::move(instruction);
}
