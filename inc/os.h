#ifndef OS_H
#define OS_H

#include <list>
#include <mutex>
#include <string>
#include <thread>

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

    void ls();

    void screenR(std::string processName);

    void screenS(std::string processName);

    void setGenerateDummyProcesses(bool value);

    /** @return True if the OS is running */
    bool isRunning();

    /**
     * Disables the running state so that the main thread can gracefully exit at
     * the end of the current clock cycle, which then joins with the calling
     * thread to allow for exiting the program.
     */
    void exit();

  private:
    int cycle;

    Scheduler scheduler;

    Config &config;

    DisplayManager &dm;

    std::vector<Core> cores;

    std::list<std::pair<int, int>> finishedProcesses;

    std::queue<std::thread> threads;

    std::thread thread;

    bool generateDummyProcesses = false;

    bool running = false;

    std::mutex mutex;

    // :wheelchair:
    // first int is ID, second int is num of cycles
    // that's right, i'll dig a hole, and then i'll bury myself in it, and then
    // i'll reflect on it there
};

#endif
