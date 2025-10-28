#ifndef PROCESS_H
#define PROCESS_H

#include <any>
#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

enum InstructionID { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

enum ProcessState {
    // NEW,
    READY,     // Process is in ready queue
    WAITING,
    RUNNING,   // Process is in a running core
    TERMINATED // Process has terminated
};

struct Instruction {
    enum InstructionID id;

    std::vector<std::any> args;
};

class Process
{
  public:
    /** Creates a new Process. Must call randomizeInstructions() */
    Process(uint32_t instruction_count);

    /** Pops an instruction and execute it (read ID and use a switch-case) */
    void execute();

    /** Increments the cycle count for this process */
    void incrementCycles();

  private:
    void _PRINT(std::vector<std::any> &args);

    void _DECLARE(std::vector<std::any> &args);

    void _ADD(std::vector<std::any> &args);

    void _SUBTRACT(std::vector<std::any> &args);

    void _SLEEP(std::vector<std::any> &args);

    void _FOR(std::vector<std::any> &args);

    /**
     * Creates a random set of instructions
     *
     * @param instruction_count The number of instructions to generate
     */
    void randomizeInstructions(int instruction_count);

    /**
     * Create a single random instruction.
     *
     * @param depth Begins at zero by default. Used to limit the depth of the
     *              FOR instruction
     *
     * @return The newly-created instruction
     */
    Instruction createInstruction(int depth = 0);

    /**
     * Generates a random variable name composed of one letter followed by a
     * numeric value from 0 to 999, e.g. "x123"
     *
     * @param uniqueness  Whether the variable name should be unique within the
     *                    process
     *
     * @return The generated variable name
     */
    std::string generateVariableName(bool uniqueness = false);

    /**
     * Helper that extracts an value of type uint16_t from an argument of type
     * std::any, either through direct conversion or by fetching a uint16_t
     * value from a variable in the process.
     *
     * @param arg The argument value
     *
     * @return A value of type uint16_t representing the argument
     */
    uint16_t getArgValueUINT16(const std::any &arg);

    /** Process ID */
    int id;

    /** Process state. Initialized to READY */
    enum ProcessState state;

    /** Number of cycles the process has executed */
    int cycles;

    /** Number of cycles the process has been in a busy-waiting state */
    int waitingTime;

    int sleepTicks;

    /** List of instructions to execute */
    std::queue<Instruction> instructions;

    /** List of variables, will not be released until process ends */
    std::unordered_map<std::string, uint16_t> variables;
};

#endif
