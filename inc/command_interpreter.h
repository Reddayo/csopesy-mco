#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <curses.h>
#include <string>

#include "../inc/ascii_map.h"
#include "../inc/display_manager.h"
#include "../inc/marquee.h"

// Define a more descriptive type alias for command argument vector
using CommandArguments = const std::vector<std::string>;

struct Command {
    /**
     * The number of parameters for this command. Ignored if allowSpaces is
     * true.
     */
    int paramCount;

    /** Set to true to treat whitespaces as part of the argument. */
    bool allowSpaces;

    /** The function to call upon executing the command. */
    std::function<void(CommandArguments)> execute;
};

class CommandInterpreter
{
  public:
    /**
     * Creates a new command interpreter instance.
     *
     * @param dm The DisplayManager handling the terminal windows
     */
    CommandInterpreter(DisplayManager &dm);

    /** Begins the user input loop for this interpreter */
    void startInputs();

    /** Exits the user input loop for this interpreter */
    void exitInputs();

    /* Adds a new command to the interpreter */
    void addCommand(std::string name, // Name of command
                    int paramCount,   // Number of parameters
                    bool allowSpaces, // Treat whitespaces as arg
                    std::function<void(CommandArguments &)> execute);

  private:
    DisplayManager &dm;

    std::unordered_map<std::string, Command> commandMap;

    bool running;
};

#endif
