#ifndef OS_H
#define OS_H

#include "config.h"
#include "display_manager.h"
#include "scheduler.h"

class OS
{
  public:
    OS(DisplayManager &dm, Config &config);

    /** OS main loop */
    void run();

    void incrementCycles();

    void resetCycles();

  private:
    int cycle;

    // Scheduler scheduler;

    Config &config;

    DisplayManager &dm;

    Core cores[];
};

#endif
