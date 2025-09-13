#include <functional>
#include <thread>

#include <curses.h>

#include "../inc/process.h"

Process::Process (WINDOW *outWindow,
                  WINDOW *inputWindow,
                  std::function<void()> execution)
{
    this->outWindow = outWindow;
    this->inputWindow = inputWindow;

    this->thread = std::thread([execution] () { execution(); });
}

Process::~Process () { (this->thread).join(); }