#ifndef PROCESS_H
#define PROCESS_H

#include <any>
#include <queue>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

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

    // Getter
    /** Returns ID */
    int getId();

    /** Returns number of finished cycles */
    int getNumFinCycles();

    /** Returns number of cycles */
    int getNumCycles();

  private:
    void _PRINT(std::vector<std::any> args);

    void _DECLARE(std::vector<std::any> args);

    void _ADD(std::vector<std::any> args);

    void _SUBTRACT(std::vector<std::any> args);

    void _SLEEP(std::vector<std::any> args);

    void _FOR(std::vector<std::any> args);

    void _ADD(std::vector<std::any> &args);

    /** Creates a random set of instructions */
    void randomizeInstructions(int instruction_count);

    /** Create a random instruction */
    Instruction createInstruction(int depth = 0);

    std::string generateVariableName(int uniqueness = 0);

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

    /** List of variables, will not be released until process ends */
    std::unordered_map<std::string, uint16_t> variables;
 
};

#endif
