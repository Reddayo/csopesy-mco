#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <curses.h>
#include <string>

#include "../inc/ascii_map.h"
#include "../inc/display_manager.h"
#include "../inc/marquee.h"


struct Command {
    int paramCount;
    std::function<void(const std::vector<std::string>&)> func;
};

class CommandInterpreter
{
  public:
    CommandInterpreter(DisplayManager &dm, Marquee &marquee);
    void start();
  private:
    Marquee &marquee;
    DisplayManager &dm;
    std::unordered_map<std::string, Command> commandMap;
    bool running;
    
};

#endif
