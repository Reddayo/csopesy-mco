#ifndef CORE_H
#define CORE_H

#include "process.h"

class Core
{
  public:
    Core();

    /** Returns the process on this core */
    Process &getProcess();

    /** Sets the process running on this core */
    void setProcess(Process &process);

    /** Calls this->process.execute() to execute one instruction */
    void execute();

    bool isRunning();

  private:
    /** Whether this core is currently running */
    bool running;

    /** The process currently executing on this core */
    Process &process;
};

#endif
