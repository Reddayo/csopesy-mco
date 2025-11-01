#ifndef CORE_H
#define CORE_H

#include <memory>

#include "process.h"

class Core
{
  public:
    Core(int id);

    /**
     * @return The shared pointer to the Process owned by this core. Establishes
     * SHARED OWNERSHIP of the pointer.
     */
    std::shared_ptr<Process> getProcess();

    /**
     * @return A reference to the shared pointer to the Process owned by this
     * core. WARNING: This effectively creates an alias to the shared pointer,
     * making it potentially thread-unsafe.
     */
    std::shared_ptr<Process> &getProcessReference();

    /**
     * Sets the process running on this core. Accepts a reference to a shared
     * pointer to a Process and CLAIMS OWNERSHIP of that pointer.
     *
     * @param process A reference to a shared pointer to a Process
     */
    void setProcess(std::shared_ptr<Process> &process);

    /** @return The core's ID */
    int getId();

    /** @return True if core is set to a running state */
    bool isRunning();

    /** Sets running state */
    void setRunning(bool running);

  private:
    int id;

    bool running;

    /** A shared pointer to the Process currently executing on this core */
    std::shared_ptr<Process> process;
};

#endif
