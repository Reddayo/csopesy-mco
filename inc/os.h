#ifndef OS_H
#define OS_H

#include "config.h"
#include "display_manager.h"
#include "scheduler.h"
#include <list>

class OS
{
  public:
    OS(DisplayManager &dm, Config &config);

    /** OS main loop */
    void run();

    void incrementCycles();

    void resetCycles();

    void ls();

  private:
    int cycle;

    Scheduler scheduler;

    Config &config;

    DisplayManager &dm;

    std::vector<Core> cores;

    std::list<std::pair<int, int>> finishedProcesses;

    // :wheelchair:
    // first int is ID, second int is num of cycles
    // that's right, i'll dig a hole, and then i'll bury myself in it, and then
    // i'll reflect on it there
};

#endif
