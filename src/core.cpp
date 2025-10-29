#include "../inc/core.h"

Core::Core (int id) : id(id), running(false) {}

Process &Core::getProcess () { return this->process; };

void Core::setProcess (Process &process) { this->process = process; }

// TODO: Remove?
void Core::execute () {}

int Core::getId () { return this->id; }

bool Core::isRunning () { return this->running; }

void Core::setRunning (bool running) { this->running = running; }
