#include "../inc/process.h"
#include <cstdint>
#include <iostream>

// TODO: If no instructions left in queue, set status to TERMINATED
void Process::execute (DisplayManager &dm)
{
    Instruction instr = instructions[this->programCounter];

    switch (instr.id) {

    case DECLARE:
        _DECLARE(instr.args);
        break;
    case ADD:
        _ADD(instr.args);
        break;
    case SUBTRACT:
        _SUBTRACT(instr.args);
        break;
    case SLEEP:
        _SLEEP(instr.args);
        break;
    case FOR:
        _FOR(instr.args);
        break;
    default:
        break;
    }

    this->programCounter++;

    if (this->programCounter >= this->instructions.size()) {
        this->setState(TERMINATED);
    }
}

uint16_t Process::getArgValueUINT16 (std::any &arg)
{
    if (arg.type() == typeid(std::string)) {
        return this->variables[std::any_cast<std::string>(arg)];
    } else {
        return std::any_cast<uint16_t>(arg);
    }
};

// TODO: This
void Process::_PRINT (std::vector<std::any> &args) {
    print_stream << std::any_cast<std::string>(args.at(0));
}

void Process::_DECLARE (std::vector<std::any> &args)
{
    variables[std::any_cast<std::string>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]));
}

void Process::_ADD (std::vector<std::any> &args)
{
    variables[std::any_cast<std::string>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]) + getArgValueUINT16(args[2]));
}

void Process::_SUBTRACT (std::vector<std::any> &args)
{
    variables[std::any_cast<std::string>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]) - getArgValueUINT16(args[2]));
}

void Process::_SLEEP (std::vector<std::any> &args)
{
    // this->state = WAITING;
    // this->elapsedWaitingCycles = getArgValueUINT16(args[1]);
}

// TODO: This
void Process::_FOR (std::vector<std::any> &args) {}
