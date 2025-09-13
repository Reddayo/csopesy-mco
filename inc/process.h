#ifndef PROCESS_H
#define PROCESS_H

#include <functional>
#include <thread>

#include <curses.h>

class Process
{
  public:
    Process(WINDOW *outWindow,
            WINDOW *inputWindow,
            std::function<void()> execution);

    ~Process();

  private:
    WINDOW *outWindow;
    WINDOW *inputWindow;
    std::thread thread;
};

#endif