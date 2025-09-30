#include <curses.h>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../inc/command_interpreter.h"
#include "../inc/display_manager.h"

CommandInterpreter::CommandInterpreter(DisplayManager &dm) : dm(dm) {};

void CommandInterpreter::startInputs ()
{
    // C-style char buffer to hold user input
    char buffer[100] = "";

    // Track the current size of the buffer
    size_t size = 0;

    // Show the command input prompt
    this->dm.showInputPrompt();

    // Used for tokenizing data from user input buffer
    std::istringstream istream;
    std::string commandName;

    this->running = true;

    while (this->running) {
        // Create command arguments vector
        std::string commandArg;
        std::vector<std::string> commandArgs;

        if (this->dm._wgetnstr(buffer, 100, size) != INPUT_READ_SUBMIT) {
            // Don't process the contents of the input buffer until user submits
            continue;
        }

        // Convert input buffer into an istringstream, then extract first token
        istream = std::istringstream(buffer);
        istream >> commandName;

        // Match command with list of valid commands
        auto commandData = commandMap.find(commandName);
        if (commandData != commandMap.end()) {
            // allowSpaces combines all arguments into one single argument
            // containing whitespace
            if (commandData->second.allowSpaces) {
                // Discard leading whitespace and read the rest of the line
                std::getline(istream >> std::ws, commandArg);
                commandArgs.push_back(commandArg);
            } else {
                // Extract the remaining tokens from the istringstream into args
                while (istream >> commandArg) {
                    commandArgs.push_back(commandArg);
                };
            }

            // Check if argument count matches
            if (commandArgs.size() != commandData->second.paramCount) {
                this->dm.showErrorPrompt("Invalid number of parameters");
            } else {
                try {
                    commandData->second.execute(commandArgs);
                } catch (const std::invalid_argument &e) {
                    this->dm.showErrorPrompt(e.what());
                } catch (const std::out_of_range &e) {
                    this->dm.showErrorPrompt(e.what());
                }
            }
        } else {
            // Matching failed
            this->dm.showErrorPrompt("Unknown command");
        }

        this->dm.showInputPrompt();
    }
}

void CommandInterpreter::exitInputs () { this->running = false; }

void CommandInterpreter::addCommand (
    std::string name,
    int paramCount,
    bool allowSpaces,
    std::function<void(CommandArguments &)> execute)
{
    // paramCount must be 1 if allowSpaces is true
    if (paramCount != 1 && allowSpaces) {
        throw std::invalid_argument(
            "paramCount must be 1 when allowSpaces is true");
    }

    // Get command data from parameters
    std::pair<std::string, Command> commandData(
        name, Command({.paramCount = paramCount,
                       .allowSpaces = allowSpaces,
                       .execute = execute}));

    // Add to command map
    this->commandMap.insert(commandData);
}
