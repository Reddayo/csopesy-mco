#include <memory>

#include "../inc/core.h"

Core::Core (int id) : id(id), running(false) {}

std::shared_ptr<Process> &Core::getProcessReference ()
{
    return this->process;
};

// Different from the other one because this shares ownership of the pointer
std::shared_ptr<Process> Core::getProcess () { return this->process; }

// TODO: Can handle setting process status here?
void Core::setProcess (std::shared_ptr<Process> &process)
{
    this->process = std::move(process);
}

// TODO: Remove?
void Core::execute () {}

int Core::getId () { return this->id; }

bool Core::isRunning () { return this->running; }

void Core::setRunning (bool running) { this->running = running; }
