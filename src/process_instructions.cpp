#include "../inc/process.h"

void Process::execute ()
{
    if (this->sleepTicks > 0)
        return;

    Instruction instr = this->instructions.front();
    this->instructions.pop();

    switch (instr.id) {
    case PRINT:
        _PRINT(instr.args);
        break;
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
    }
}

uint16_t Process::getArgValueUINT16 (const std::any &arg)
{
    if (arg.type() == typeid(std::string)) {
        return this->variables[std::any_cast<std::string &>(arg)];
    } else {
        return std::any_cast<uint16_t>(arg);
    }
};

void Process::_DECLARE (std::vector<std::any> &args)
{
    variables[std::any_cast<const std::string &>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]));
}

void Process::_ADD (std::vector<std::any> &args)
{
    variables[std::any_cast<const std::string &>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]) + getArgValueUINT16(args[2]));
}

void Process::_SUBTRACT (std::vector<std::any> &args)
{
    variables[std::any_cast<const std::string &>(args[0])] =
        (uint16_t)(getArgValueUINT16(args[1]) - getArgValueUINT16(args[2]));
}

void Process::_SLEEP (std::vector<std::any> &args)
{
    this->state = WAITING;
    this->sleepTicks = getArgValueUINT16(args[1]);
}
