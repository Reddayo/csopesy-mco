#include "../inc/core.h"

Core::Core (int id) : id(id) {}

void Core::setProcess (Process &process) { this->process = process; }

void Core::execute () { this->process.execute(); }

int Core::getId () { return this->id; }

bool Core::isRunning () { return this->running; }

void Core::setRunning (bool running) { this->running = running; }
