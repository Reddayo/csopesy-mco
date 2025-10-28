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
    NEW,       // Mostly unused, except for "empty" Process
    READY,     // Process is in ready queue
    WAITING,   // Process is in a waiting state (busy-waiting)
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
    /**
     * Default constructor for a Process. Used to create an "empty" Process
     * with which a Core can be initialized
     */
    Process();

    /** Creates a new Process. Must call randomizeInstructions() */
    Process(int id, uint32_t instruction_count);

    /** Pops an instruction and execute it (read ID and use a switch-case) */
    void execute();

    /** Increments the cycle count for this process */
    void incrementCycles();

    /** @return ID */
    int getId();

    /** @return Number of finished cycles */
    uint32_t getElapsedCycles();

    /** @return Number of cycles */
    uint32_t getNumCycles();

    /** @return Number of cycles this process has been sleeping for */
    uint16_t getSleepTicks();

    /** Decrement sleep timer */
    void decrementSleepTicks();

  private:
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

    /** Process ID */
    int id;

    /** Process state. Initialized to READY */
    enum ProcessState state;

    /** Number of cycles the process has executed */
    uint32_t elapsedCycles;

    /** Number of cycles the process has been in a busy-waiting state */
    uint32_t waitingTime;

    uint16_t sleepTicks;

    /** List of instructions to execute */
    std::queue<Instruction> instructions;

    /** List of variables, will not be released until process ends */
    std::unordered_map<std::string, uint16_t> variables;
};

#endif
