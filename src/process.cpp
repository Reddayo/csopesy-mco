#include <cstdint>
#include <iostream>
#include <random>

#include "../inc/process.h"
#include "process.h"

Process::Process() {
    /** followed @Red's implementation of randomizeInstructions() */
    randomizeInstructions(rand() % 10 + 1);

    /** ...is this fr */
    this->id = id++;
    this->state = READY;
}

void
Process::_SUBTRACT(std::vector<std::any>& args){
    
  
    /** _SUBTRACT var1, var2/value, var3/value
     *  all are uint16
     */

   auto getArgValue = [&](const std::any& a) -> uint16_t {
        if (a.type() == typeid(std::string)) {
            return variables[std::any_cast<const std::string&>(a)];
        } else {
            return std::any_cast<uint16_t>(a);
        }
    };

    variables[std::any_cast<const std::string&>(args[0])] = (uint16_t) (getArgValue(args[1]) - getArgValue(args[2]));
}

/* PAUSED
void
Process::_FOR(std::vector<std::any>& args){
    
    auto getIntValue = [&](const std::any& a) -> uint16_t {
        if (a.type() == typeid(std::string)) {
            return variables[std::any_cast<const std::string&>(a)];
        } else {
            return std::any_cast<uint16_t>(a);
        }
    };

    for(int i = 0; i < getIntValue(args[1]); i++) {
    
    }
}

*/