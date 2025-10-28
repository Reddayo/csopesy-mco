#include <cstdint>
#include <iostream>
#include <random>

#include "../inc/process.h"

Process::Process ()
{
    /** followed @Red's implementation of randomizeInstructions() */
    randomizeInstructions(rand() % 10 + 1);

    /** ...is this fr */
    this->id = id++;
    this->state = READY;
}

void Process::_SUBTRACT (std::vector<std::any> &args)
{

    /** _SUBTRACT var1, var2/value, var3/value
     *  all are uint16
     */

    auto getArgValue = [&] (const std::any &a) -> uint16_t {
        if (a.type() == typeid(std::string)) {
            return variables[std::any_cast<const std::string &>(a)];
        } else {
            return std::any_cast<uint16_t>(a);
        }
    };

    variables[std::any_cast<const std::string &>(args[0])] =
        (uint16_t)(getArgValue(args[1]) - getArgValue(args[2]));
}

void Process::incrementCycles ()
{
    /** Increment the cycle count.... is this it fr? */
    cycles++;
}

void Process::_ADD (std::vector<std::any> &args)
{

    /** _ADD var1, var2/value, var3/value
     *  all are uint16
     */

    auto getArgValue = [&] (const std::any &a) -> uint16_t {
        if (a.type() == typeid(std::string)) {
            return variables[std::any_cast<const std::string &>(a)];
        } else {
            return std::any_cast<uint16_t>(a);
        }
    };

    variables[std::any_cast<const std::string &>(args[0])] =
        (uint16_t)(getArgValue(args[1]) + getArgValue(args[2]));
}

/** Count is randomly assigned by the scheduler*/
void Process::randomizeInstructions (int instruction_count)
{

    /** PRINT -   msg = "Hello World from {process_name}" // The “msg” can print
     * 1 variable, “var.” E.g. PRINT (“Value from: ” +x) DECLARE - var1, default
     * value = 0 ADD     - var1, var2/uint16, var3/uint16, variables are
     * declared 0 if they aren't declared beforehand SUB     - var1,
     * var2/uint16, var3/uint16 SLEEP   - uint8 FOR     - [instructions],
     * repeats n times
     */
    for (int i = 0; i < instruction_count; i++) {
        instructions.push(createInstruction());
    }
}

/** Uniqueness is not enforced by default*/
std::string Process::generateVariableName (int uniqueness)
{

    std::string name;

    do {
        name =
            std::string(1, 'a' + (rand() % 26)) + std::to_string(rand() % 1000);
    } while (uniqueness == 1 && variables.count(name));

    return name;
}

Instruction Process::createInstruction (int depth)
{

    Instruction instruction;
    /** If this is the 3rd loop, randomly pick a non for instruction */
    instruction.id =
        static_cast<InstructionID>(rand() % (depth > 0 && depth < 3 ? 5 : 6));

    switch (instruction.id) {

    /* TODO: Fix once print function is done */
    case PRINT: {
        instruction.args.push_back(
            std::string("Hello world from " + std::to_string(id) + "!"));
        break;
    }
    /* Declare, declares a unique variable... I'm assuming*/
    case DECLARE: {
        std::string var = generateVariableName(1);
        variables[var] = 0;
        instruction.args.push_back(var);
    }
    /** TODO: Is add different to subtract?
     * ADD said all variables that hasn't been declared should be declared as 0.
     * But SUB doesn't say that
     */
    case ADD:
    case SUBTRACT: {
        /** Var1 is always a destination variable*/
        std::string var1 = generateVariableName();
        if (!variables.count(var1))
            variables[var1] = 0;
        /** Var2 and var3 can be either  uint16 value or a variable*/
        auto createSource = [&] () -> std::any {
            if (rand() % 2 == 0) {
                return (uint16_t)(rand() % 65536);
            } else {
                std::string name = generateVariableName();
                if (!variables.count(name))
                    variables[name] = 0;
                return name;
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
        int n = rand() % 256 +
                1; // So it doesn't repeat 0 times. No specified repeat times
        instruction.args = {createInstruction(depth + 1), n};
        break;
    }
    }

    return instruction;
}
