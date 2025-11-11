#ifndef PROCESS_H
#define PROCESS_H

#include <any>
#include <cstdint>
#include <ctime>
#include <functional>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "display_manager.h"
#include "memory_manager.h"

enum InstructionID { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR, READ, WRITE };

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

struct Page {
    int frameNumber;   // which frame in RAM this page is mapped to
    bool present;      // True if the page is currently in physical memory
    bool dirty;        // True if the page has been modified since being loaded
};


class Process
{
  public:
    /** Creates a new Process. Must call randomizeInstructions() */                     // vvv this is probably bad
    Process(int id, std::string name, uint32_t instruction_count, uint32_t memory_size, uint32_t mem_per_frame);

    /** Pops an instruction and execute it (read ID and use a switch-case) */
    void execute(MemoryManager &mm);

    /** @return The process name */
    std::string getName();

    /** @return The process id */
    int getId();

    /** @return The process start time */
    std::time_t getStartTime();

    /** Sets the process state */
    void setState(enum ProcessState state);

    /** @return The process sate */
    enum ProcessState getState();

    /** @return the value of the program counter */
    uint32_t getProgramCounter();

    /** @return Total number of cycles to finish the program */
    uint32_t getTotalCycles();

    // Elapsed cycles
    // These methods have to do with the # of cycles that have elapsed since the
    // process was created

    /** @return Number of finished cycles since process was last dispatched */
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

    /** @return Number of cycles this process has been in busy-waiting */
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

    /** @return A string summary of the process' state, including logs */
    std::string getStateAsString();

    void setLastCoreID(uint32_t core);

    void setTotalCycles(int totalCycles);

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

    void _PRINT(std::vector<std::any> &args, MemoryManager &mm);

    void _DECLARE(std::vector<std::any> &args, MemoryManager &mm);

    void _ADD(std::vector<std::any> &args, MemoryManager &mm);

    void _SUBTRACT(std::vector<std::any> &args, MemoryManager &mm);

    void _SLEEP(std::vector<std::any> &args, MemoryManager &mm);

    void _FOR(std::vector<std::any> &args, MemoryManager &mm);

    void _READ(std::vector<std::any> &args, MemoryManager &mm);

    void _WRITE(std::vector<std::any> &args, MemoryManager &mm);

    /**
     * Creates a random set of instructions
     *
     * @param instruction_count The number of instructions to generate
     */
    void randomizeInstructions(int instruction_count);

    /**
     * Create a single random instruction and relinquishes ownership to a unique
     * pointer to it
     *
     * @param depth Begins at zero by default. Used to limit the depth of the
     *              FOR instruction
     * @param instCtr   Instruction counter
     *
     * @param instruction_count   Instruction count to limit number of
     * instructions
     *
     * @param loopCount   Number of times an instruction will be looped. If no
     * FOR loop, set to 1.
     *
     * @return A unique pointer to a new instruction
     */
    std::shared_ptr<Instruction> createInstruction(int depth,
                                                   int *instCtr,
                                                   int instruction_count,
                                                   int loopCount);

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

    /** Process name */
    std::string name;

    /** Process ID */
    int id;

    /** Process time of creation */
    std::time_t startTime;

    /** Total number of instructions */
    int totalCycles;

    /** Process state. Initialized to READY */
    enum ProcessState state;

    /** Number of cycles the process has been running, reset by pre-emption */
    uint32_t elapsedCycles = 0;

    /** Number of cycles the process has been in a busy-waiting state */
    uint32_t remainingBusyWaitingCycles = 0;

    /** Number of cycles the process has been in a waiting state (SLEEP) */
    uint16_t remainingWaitingCycles = 0;

    /** Program counter */
    int programCounter = 0;

    /** List of instructions to execute */
    std::vector<std::shared_ptr<Instruction>> instructions;

    /** List of variables, will not be released until process ends */
    /** Variables to address */
    std::unordered_map<std::string, uint32_t> variables;

    // tbh you can pass uint16_t - 1, for better memory management but nah
    uint32_t memory_size;             // memory of the process
    uint32_t mem_per_frame;           // memory per page
    uint16_t requiredPages;           // required pages of the process
 
    // the max is 65535, maxed starting mappable address is whatever the memory_size is - 2
    uint32_t logicalAddressCounter;

    std::stringstream print_stream;

    /** The most recent core ID this process was assigned to. Defaults to 0 */
    uint32_t core;

    // std::vector<std::string> declaredVariableNames;
};

#endif
