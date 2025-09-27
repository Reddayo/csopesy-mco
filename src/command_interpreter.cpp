#include <curses.h>
#include <string>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "../inc/ascii_map.h"
#include "../inc/display_manager.h"
#include "../inc/marquee.h"
#include "../inc/command_interpreter.h"

CommandInterpreter::CommandInterpreter(DisplayManager &dm, Marquee &marquee) : dm(dm), marquee(marquee){
    auto startMarquee = [this](const std::vector<std::string>&){ this->marquee.start(); };
    auto stopMarquee  = [this](const std::vector<std::string>&){ this->marquee.stop(); };
    auto refreshCmd   = [this](const std::vector<std::string>&){ this->dm.refreshAll(); };
    auto helpCmd      = [this](const std::vector<std::string>&){
            // Pause the animation, clear the output window, and show help.
            this->marquee.stop();
            this->dm.clearOutputWindow();
            this->dm._mvwprintw(
                0, 0, "%s",
                "help           - displays the commands and its description\n"
                "start_marquee  - starts the marquee animation\n"
                "stop_marquee   - stops the marquee animation\n"
                "set_text       - accepts a text input and displays it as a "
                "marquee\n"
                "set_speed      - sets the marquee animation refresh in "
                "milliseconds\n"
                "exit           - terminates the console\n"
                "refresh        - refresh the windows");
            // By this point, calling marquee.start() will resume the animation
    };
    auto exitCmd      = [this](const std::vector<std::string>&){ 
        this->marquee.stop(); 
        this->running = false; 
    };
    auto setTextCmd   = [this](const std::vector<std::string>& args){ this->marquee.setText(args[0]); };
    auto setSpeedCmd  = [this](const std::vector<std::string>& args){
        this->marquee.setRefreshDelay(std::stoi(args[0]));
    };
    
    this->commandMap = {
        {"start_marquee", {0, startMarquee}},
        {"stop_marquee",  {0, stopMarquee}},
        {"refresh",       {0, refreshCmd}},
        {"help",          {0, helpCmd}},
        {"exit",          {0, exitCmd}},
        {"set_text",      {1, setTextCmd}},
        {"set_speed",     {1, setSpeedCmd}}
    };
}

void
CommandInterpreter::start(){

    // Main loop for fetching input

    // C-style char buffer to hold user input
    char buffer[100] = "";

    size_t size = 0;

    int read;
    
    dm.showInputPrompt();


    running = true;
    while (running) {
        read = dm._wgetnstr(buffer, 100, size);

        if (read != INPUT_READ_SUBMIT) {
            // Don't process the contents of the input buffer until user submits
            continue;
        }

        std::string line(buffer);
        std::istringstream iss(line);
        std::string cmd;
        std::vector<std::string> args;

        iss >> cmd;
        std::string arg;
        while (iss >> arg) args.push_back(arg);

        auto it = commandMap.find(cmd);
        if (it != commandMap.end()) {
            if (cmd == "set_text" && args.size() != it->second.paramCount) {
                std::string full = "";
                for (const std::string s : args) {
                    full += ' ';
                    full += s;
                }
                args[0] = full;

                it->second.func(args);
            } else if (args.size() != it->second.paramCount) {
                dm.showErrorPrompt("Invalid number of parameters");
            } else {
                try {
                    it->second.func(args);
                } catch (const std::invalid_argument &e){
                    dm.showErrorPrompt("Set_speed expects an integer value input");
                } catch (const std::out_of_range &e) {
                    dm.showErrorPrompt("Set_speed paramater is outside the range of an integer");
                }  
            }
        } else {
            dm.showErrorPrompt("Unknown command");
        }

        dm.showInputPrompt();
    }
}
