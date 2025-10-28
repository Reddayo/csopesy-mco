#include <cstdint>
#include <iostream>
#include <random>

#include "../inc/process.h"
#include "process.h"


Process::Process ()
{
    /** followed @Red's implementation of randomizeInstructions() */
    randomizeInstructions(rand() % 10 + 1);

    /** ...is this fr */
    this->id = id++;
    this->state = READY;

    this->sleep_ticks = 0;
}

bool
Process::execute() { 
    //if (sleepticks > 0) return false;

    Instruction instr = instruction.front();
    instruction.pop();

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
    return false;
 }

void 
Process::_DECLARE (std::vector<std::any>& args){

    /** _DECLARE var, value
    *   all are uint16
    */
    auto getArgValue = [&] (const std::any &a) -> uint16_t {
        if (a.type() == typeid(std::string)) {
            return variables[std::any_cast<const std::string &>(a)];
        } else {
            return std::any_cast<uint16_t>(a);
        }
    };

    variables[std::any_cast<const std::string &>(args[0])] = (uint16_t) (getArgValue(args[1])
}

void 
Process::_SLEEP (int x) {

    /** _SLEEP x
     *   uint8
     */
    this->state = WAITING;

    this.sleep_ticks = x;
}
