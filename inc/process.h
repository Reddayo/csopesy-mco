#ifndef PROCESS_H
#define PROCESS_H

#include <any>
#include <queue>
#include <vector>

enum InstructionID { PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR };

enum ProcessState {
    // NEW,
    READY, // Process is in ready queue
    // WAITING,
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
    /** Must call randomizeInstructions() */
    Process();

    /** Pops an instruction and execute it (read ID and use a switch-case) */
    void execute();

    /** Increments the cycle count for this process */
    void incrementCycles();

  private:
    void _PRINT(std::vector<std::any> args);

    void _DECLARE(std::vector<std::any> args);

    void _ADD(std::vector<std::any> args);

    void _SUBTRACT(std::vector<std::any> args);

    void _SLEEP(std::vector<std::any> args);

    void _FOR(std::vector<std::any> args);

    /** Creates a random set of instructions */
    void randomizeInstructions();

    /** Process ID */
    int id;

    /** Initialized to READY */
    enum ProcessState state;

    /** Number of cycles the process has executed */
    int cycles;

    /** Number of cycles the process has been in a busy-waiting state */
    int waitingTime;

    /** List of instructions to execute */
    std::queue<Instruction> instructions;
};

#endif
