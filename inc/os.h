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

    void updateConfig(Config &config);
    void incrementCycles();

    void resetCycles();

    /**
     * screen -ls command
     *
     * @param writeReport True for report-util command
     */
    void ls(bool writeReport);

    void screenR(std::string processName);

    void screenS(std::string processName);

    void processSMI();

    void setGenerateDummyProcesses(bool value);

    /** @return True if the OS is running */
    bool isRunning();

    /** @return True if the scheduler is generating processes */
    bool isGenerating();

    /**
     * Disables the running state so that the main thread can gracefully exit at
     * the end of the current clock cycle, which then joins with the calling
     * thread to allow for exiting the program.
     */
    void exit();

    void reset();

  private:
    int cycle;

    int processAutoId = 0;

    Scheduler scheduler;

    Config &config;

    DisplayManager &dm;

    std::vector<Core> cores;

    std::list<std::pair<std::string, int>> finishedProcesses;

    std::queue<std::thread> threads;

    std::thread thread;

    bool generateDummyProcesses = false;

    bool running = false;

    std::mutex mutex;

    /** The current process loaded into the process screen */
    std::shared_ptr<Process> loadedProcess;

    void showDefaultProcessScreenMessage();

    /**
     * Finds an ongoing process by name and shares ownership of the pointer to
     * that process
     */
    std::shared_ptr<Process> findOngoingProcessByName(
        const std::string &processName);
};

#endif
