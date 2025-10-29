#ifndef PROCESS_H
#define PROCESS_H

#include <any>
#include <cstdint>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include "display_manager.h"

enum InstructionID { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

enum ProcessState {
    NEW,       // Mostly unused, except for "empty" Process
    READY,     // Process is in ready queue
    WAITING,   // Process is in a waiting state (SLEEP instruction)
    RUNNING,   // Process is in a running core (executing or busy-waiting)
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
    Process(int id, uint32_t instruction_count);

    /** Destructor for a Process */
    ~Process();

    /** Pops an instruction and execute it (read ID and use a switch-case) */
    void execute(DisplayManager &dm);

    /** @return The process ID */
    int getId();

    /** Sets the process state */
    void setState(enum ProcessState state);

    /** @return The process sate */
    enum ProcessState getState();

    /** @return Total number of cycles to finish the program */
    uint32_t getTotalCycles();

    // Elapsed cycles
    // These methods have to do with the # of cycles that have elapsed since the
    // process was created

    /** @return Number of finished cycles */
    uint32_t getElapsedCycles();

    /** Increments the cycle count for this Process */
    void incrementElapsedCycles();

    /** Resets elapsed cycles */
    void resetElapsedCycles();

    // Busy-waiting cycles
    // These methods have to do with the # of cycles that a process has been
    // in a busy-waiting state, functioning as a delay timer
    // (delay-per-exec) NOTE: Process will have a state of RUNNING even in a
    // busy-waiting state.

    uint32_t getRemainingBusyWaitingCycles();

    void setBusyWaitingCycles(uint32_t value);

    void decrementBusyWaitingCycles();

    // Sleep cycles
    // These methods have to do with the # of cycles that a process has been in
    // a waiting state after a SLEEP instruction

    /** @return Number of cycles this process has been waiting (SLEEP) */
    uint16_t getRemainingWaitingCycles();

    void setWaitingCycles(uint16_t value);

    void decrementWaitingCycles();

    uint32_t getProgramCounter();

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
    uint16_t getArgValueUINT16(std::any &arg);

    // Instruction methods
    // Implementations are in process_instructions.cpp

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

    /** Number of cycles the process has been running, reset by pre-emption */
    uint32_t elapsedCycles = 0;

    /** Number of cycles the process has been in a busy-waiting state */
    uint32_t elapsedBusyWaitingCycles = 0;

    /** Number of cycles the process has been in a waiting state (SLEEP) */
    uint16_t elapsedWaitingCycles = 0;

    /** Program counter */
    int programCounter = 0;

    /** List of instructions to execute */
    std::vector<Instruction> instructions;

    /** List of variables, will not be released until process ends */
    std::unordered_map<std::string, uint16_t> variables;
};

#endif
