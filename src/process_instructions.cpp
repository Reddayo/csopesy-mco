#include <algorithm> // for std::clamp if using C++17 TBF, i can just use if else
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <typeinfo>

#include "../inc/process.h"

void Process::execute (MemoryManager &mm)
{
    // Maintain a reference to the instruction pointer (don't claim ownership)
    std::shared_ptr<Instruction> &instr = instructions[this->programCounter];

    // Execute a different method depending on instruction ID
    switch (instr->id) {
    case PRINT:
        _PRINT(instr->args, mm);
        break;
    case DECLARE:
        _DECLARE(instr->args, mm);
        break;
    case ADD:
        _ADD(instr->args, mm);
        break;
    case SUBTRACT:
        _SUBTRACT(instr->args, mm);
        break;
    case SLEEP:
        _SLEEP(instr->args, mm);
        break;
    case FOR:
        _FOR(instr->args, mm);
        break;
    case READ:
        _READ(instr->args, mm);
        break;
    case WRITE:
        _WRITE(instr->args, mm);
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

/* Remake to handle hexadecimal memory address location */
// TODO: Haven't tested this lmao
uint16_t Process::getArgValueUINT16 (std::any &arg)
{
    if (arg.type() == typeid(std::string)) {
        std::string str = std::any_cast<std::string>(arg);

        if (str.substr(0, 2) == "0x") {
            std::istringstream iss(str);
            // TODO: Range
            unsigned int address;

            iss >> std::hex >> address;

            return address;
        }

        return this->variables[str];
    } else {
        if (arg.type() == typeid(uint32_t)) {
            // Lmao
            return std::any_cast<uint32_t>(arg);
        }

        return std::any_cast<uint16_t>(arg);
    }
};

void Process::_PRINT (std::vector<std::any> &args, MemoryManager &mm)
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

            print_stream << mm.read(this->id, variables[var_name], 2);
        }
    };

    print_stream << "\"\n";
}

/*
uint16_t Process::allocateAddress() {
    // 2-byte aligned allocation
    while (std::any_of(variables.begin(), variables.end(),
                       [this](auto &p){ return p.second == logicalAddressCounter; })) {
        logicalAddressCounter += 2;
    }
    uint16_t allocated = logicalAddressCounter;
    logicalAddressCounter += 2;
    return allocated;
}
*/

// ASSUMES THAT STUFF IS 2-BYTES ALIGNED
void Process::_DECLARE(std::vector<std::any> &args, MemoryManager &mm)
{
    if (variables.size() >= 32) {
        print_stream << "Declare instruction failed: reached 32 variables limit.\n";
        return;
    }

    std::string varName = std::any_cast<std::string>(args[0]);
    uint16_t value = static_cast<uint16_t>(getArgValueUINT16(args[1]));

    uint16_t addr;
    if (variables.count(varName) > 0) {
        // overwrite existing variable
        addr = variables[varName];
    } else {
        // assign next free address
        addr = logicalAddressCounter;
        variables[varName] = addr;
        logicalAddressCounter += 2; // only increment when allocating new
    }

    mm.write(this->id, addr, value, 2);

    /* print_stream << "Declare Success: Address: " << addr
                 << " Value: " << value << " Count: " << variables.size()
                 << " Name: " << varName << "\n";
    */
}
void Process::_ADD(std::vector<std::any> &args, MemoryManager &mm)
{
    int val1 = (args[1].type() == typeid(std::string))
                    ? mm.read(this->id, variables[std::any_cast<std::string>(args[1])], 2)
                    : std::any_cast<uint16_t>(args[1]);

    int val2 = (args[2].type() == typeid(std::string))
                    ? mm.read(this->id, variables[std::any_cast<std::string>(args[2])], 2)
                    : std::any_cast<uint16_t>(args[2]);

    int result = std::clamp(val1 + val2, 0, static_cast<int>(std::numeric_limits<uint16_t>::max()));

    std::string destVar = std::any_cast<std::string>(args[0]);
    uint16_t addr;

    if (variables.count(destVar) > 0) {
        // overwrite existing variable
        addr = variables[destVar];
    } else {
        if (variables.size() >= 32) {
            // print_stream << "ADD instruction ignored: reached 32 variable limit.\n";
            return;
        }

        // allocate new address
        addr = logicalAddressCounter;
        variables[destVar] = addr;
        logicalAddressCounter += 2;
    }

    mm.write(this->id, addr, static_cast<uint16_t>(result), 2);

    /* print_stream << "Addition instruction: " << destVar
                 << " = " << val1 << " + " << val2 << " = " << result << "\n"; */
}

void Process::_SUBTRACT(std::vector<std::any> &args, MemoryManager &mm)
{
    int val1 = (args[1].type() == typeid(std::string))
                    ? mm.read(this->id, variables[std::any_cast<std::string>(args[1])], 2)
                    : std::any_cast<uint16_t>(args[1]);

    int val2 = (args[2].type() == typeid(std::string))
                    ? mm.read(this->id, variables[std::any_cast<std::string>(args[2])], 2)
                    : std::any_cast<uint16_t>(args[2]);

    int result = std::clamp(val1 - val2, 0, static_cast<int>(std::numeric_limits<uint16_t>::max()));

    std::string destVar = std::any_cast<std::string>(args[0]);
    uint16_t addr;

    if (variables.count(destVar) > 0) {
        // overwrite existing variable
        addr = variables[destVar];
    } else {
        if (variables.size() >= 32) {
            // print_stream << "SUBTRACT instruction ignored: reached 32 variable limit.\n";
            return;
        }

        // allocate new address
        addr = logicalAddressCounter;
        variables[destVar] = addr;
        logicalAddressCounter += 2;
    }

    mm.write(this->id, addr, static_cast<uint16_t>(result), 2);

    /* print_stream << "Subtraction instruction: " << destVar
                 << " = " << val1 << " - " << val2 << " = " << result << "\n"; */
}


void Process::_SLEEP (std::vector<std::any> &args, MemoryManager &mm)
{
    // Set to WAITING so that processs gets moved to sleep queue in next cycle
    this->state = WAITING;
    this->remainingWaitingCycles = getArgValueUINT16(args[0]);
}

// TODO: This
void Process::_FOR (std::vector<std::any> &args, MemoryManager &mm)
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

void Process::_READ (std::vector<std::any> &args, MemoryManager &mm)
{
    std::string varname = std::any_cast<std::string>(args[0]);
    uint32_t address = getArgValueUINT16(args[1]);
    uint32_t var_address = variables[varname];

    uint16_t value_read = mm.read(this->id, address, 2);
    mm.write(this->id, var_address, value_read, 2);

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    /* print_stream << std::put_time(std::localtime(&now_c), "(%m/%d/%Y %H:%M:%S)")
                 << "." << std::setw(3) << std::setfill('0') << ms.count()
                 << " Reading from address " << address << " into variable "
                 << varname << " (memory location: " << var_address
                 << "):\n    Value is " << value_read << "\n"; */
}
void Process::_WRITE (std::vector<std::any> &args, MemoryManager &mm)
{
    uint32_t address = getArgValueUINT16(args[0]);
    uint32_t value = getArgValueUINT16(args[1]);

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    /* print_stream << std::put_time(std::localtime(&now_c), "(%m/%d/%Y %H:%M:%S)")
                 << "." << std::setw(3) << std::setfill('0') << ms.count()
                 << " Writing value " << value << " into address " << address
                 << "\n"; */

    mm.write(this->id, address, value,
             2);
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
             ? 8
             : 7));
    // for testing DECLARE instructions:
    // instruction->id = static_cast<InstructionID>(1);
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
        /*
        {
            // ADD(x, x, random number in [1, 10])
            instruction->args = {std::string("x"), std::string("x"),
                                 (uint16_t)(rand() % 10 + 1)};
            break;
        }
        */
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

    case READ: {
        instruction->args = {std::string("kotone"),
                             // Address in [0, 2, 4, ... 20]
                             // Test: Read from address 16
                             (uint32_t)16}; //(uint32_t)((rand() % 11) * 2)};
        break;
    }

    case WRITE: {
    }
        instruction->args = {
            // Test: Write value of 69 to address 16
            (uint32_t)16,  // (uint32_t)(rand() % this->memorySize),
            (uint16_t)69}; // (uint16_t)((rand() % 11) * 2)};
        break;
    }

    return std::move(instruction);
}
