#ifndef PROCESS_H
#define PROCESS_H

#include <functional>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>

#include <curses.h>

class Process
{
  public:
    Process(std::function<void()> execution);

    virtual ~Process();

    void start();   // Launches the thread
    void stop();    // Kills the thread
    void pause();   // Pauses the thread
    void resume();  // Resume if paused
    
    bool isRunning() const;
    bool isPaused() const;
  
  protected:
    virtual void run();
    

  private:
    void runWrapper();
    std::function<void()> execution;
    std::thread thread;
    std::atomic<bool> running{false};
    std::atomic<bool> paused{false};
    std::mutex mtx;
    std::condition_variable cv;
};

#endif