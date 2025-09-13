#include <functional>
#include <thread>

#include <curses.h>

#include "../inc/process.h"
// Uses initializer list, so that execution doesn't start immediately
Process::Process (std::function<void()> execution): execution(std::move(execution)){}

Process::~Process() {
    stop();
    if (thread.joinable()) thread.join();
}

void Process::start() {
    if (!running) {
        running = true;
        paused = false;
        thread = std::thread([this]() { this->run(); });
    }
}

void Process::stop() {
    running = false;
    resume();
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

void Process::run() {
    while (running) {

        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, [this]{ return !paused || !running; });

        lock.unlock();

        if (!running) break;

        execution();
    }
}