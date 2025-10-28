#include <cstdint>

#include "../inc/process.h"

// ARE YOU RE@DY!! I'M L@DY!! HAJIMEYOU YAREBA DEKIRU KITTO ZETTAI WATASHI #1
Process::Process (uint32_t instruction_count) : state(READY)
{
    randomizeInstructions(instruction_count);
}

void Process::incrementCycles () { cycles++; }

void Process::randomizeInstructions (int instruction_count)
{
    // NOTE: Count is randomly assigned by the scheduler
    for (int i = 0; i < instruction_count; i++) {
        this->instructions.push(createInstruction());
    }
}

std::string Process::generateVariableName (bool uniqueness)
{
    // WARNING: Uniqueness is not enforced by default
    std::string name;

    do {
        name =
            std::string(1, 'a' + (rand() % 26)) + std::to_string(rand() % 1000);
    }
    // Randomly generate names until you get a unique one
    while (uniqueness == true && variables.count(name));

    return name;
}

Instruction Process::createInstruction (int depth)
{
    Instruction instruction;

    // Avoid generating a FOR instruction beyond depth = 3
    instruction.id =
        static_cast<InstructionID>(rand() % (depth > 0 && depth < 3 ? 5 : 6));

    switch (instruction.id) {

    case PRINT: {
        break;
    }

    case DECLARE: {
        std::string var = generateVariableName(1);
        instruction.args.push_back(var);
        break;
    }

    case ADD:
    case SUBTRACT: {
        // First argument is always a destination variable
        std::string var1 = generateVariableName();

        // Other arguments can be a random variable or a random uint16_t value
        // TODO: Move to actual implementation of instructions instead of here
        auto createSource = [&] () -> std::any {
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
        instruction.args = {createInstruction(depth + 1), n};
        break;
    }
    }

    return instruction;
}
