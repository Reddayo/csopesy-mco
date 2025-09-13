#include <functional>
#include <thread>

#include <curses.h>

#include "../inc/process.h"
// Uses initializer list, so that execution doesn't start immediately
Process::Process (std::function<void()> execution, int delayMs)
                 : execution(std::move(execution)),
                   running(false),
                   paused(false), 
                   delayMs(delayMs){}

Process::~Process() {
    stop();
}

void Process::start() {
    if (!running) {
        running = true;
        paused = false;
        thread = std::thread([this]() { this->runWrapper(); });
    }
}

void Process::stop() {
    running = false;
    resume();
      if (thread.joinable()) {
        thread.join();
    }
}

void Process::pause() {
    paused = true;
}

void Process::resume() {
    if (paused) {
        paused = false;
        cv.notify_all();
    }
}

bool Process::isRunning() const {
    return running;
}

bool Process::isPaused() const {
    return paused;
}

void Process::setDelayMs(int newDelayMs){
    delayMs = newDelayMs;
}
void Process::run() {

    if (execution) {
        execution();
    }
}

void Process::runWrapper() {
    while (running) {

        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this]{ return !paused || !running; });

        if (!running) break;
        lock.unlock();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        run();
    }
}