#ifndef CORE_H
#define CORE_H

#include "process.h"

class Core
{
  public:
    Core(int id);

    /** @return The process on this core */
    Process &getProcess();

    /** Sets the process running on this core */
    void setProcess(Process &process);

    /** Calls this->process.execute() to execute one instruction */
    void execute();

    /** @return The core's ID */
    int getId();

    /** @return True if core is set to a running state */
    bool isRunning();

    /** Sets running state */
    void setRunning(bool running);

  private:
    /** ID of the core */
    int id;

    /** Whether this core is currently running */
    bool running;

    /** The process currently executing on this core */
    Process process;
};

#endif
