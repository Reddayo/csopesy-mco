#include <cstring>
#include <curses.h>
#include <string>
#include <thread>

#include "../inc/marquee.h"
#include "../inc/process.h"

const int INPUT_WINDOW_HEIGHT = 10;
const int DEFAULT_REFRESH_DELAY = 100;

int main ()
{
    int max_y, max_x;

    initscr(); // Initialize the screen
    cbreak();  // Disable line buffering
    // noecho();  // Disable input echo

    // Get the dimensions of the terminal
    // WARNING: display will break if you resize the terminal
    getmaxyx(stdscr, max_y, max_x);

    // Create window for output
    WINDOW *outWindow = newwin(max_y - INPUT_WINDOW_HEIGHT, max_x, 0, 0);

    // Create window for user input
    WINDOW *inputWindow =
        newwin(INPUT_WINDOW_HEIGHT, max_x, max_y - INPUT_WINDOW_HEIGHT, 0);

    // Draw borders around the windows
    box(outWindow, 0, 0);
    box(inputWindow, 0, 0);

    wrefresh(outWindow);
    wrefresh(inputWindow);

    // TODO: When capturing variables into lambda scope:
    // Is passing by reference better?

    // curses requires a C-style char buffer to hold the thing
    char buffer[100] = "";
    


    // Create a new "process" to start the marquee, idk
    Marquee marquee(outWindow, max_x - 2);

    bool running = true;
    while (running) {
        mvwgetstr(inputWindow, 1, 1, const_cast<char *>(buffer));

        // Where the commands go    
        std::string command(buffer);

        if (command == "help") {
            
        } else if (command == "start_marquee") {
            marquee.start();
        } else if (command == "stop_marquee") {
            marquee.stop();
        } else if (command.rfind("set_text ", 0) == 0) {
            marquee.setText(command.substr(9));
        } else if (command.rfind("set_speed ", 0) == 0) {
            marquee.setRefreshDelay(std::stoi(command.substr(10)));
        } else if (command == "exit") {
            marquee.stop();
            running = false;
            break;
        }

    }
    endwin();

    /*
    // Currently prompts a string from the user and plays a marquee animation
    std::thread t2([buffer, inputWindow, outWindow,  max_x]() {
        while(1) {
            mvwgetstr(inputWindow, 1, 1, const_cast<char*>(buffer));
            // mvwprintw(outWindow, 10, 1, buffer);
            std::thread t1([buffer, outWindow, max_x]() {
                std::string text(buffer);
                startMarquee(outWindow, text, 10, max_x - 2);
            });

            // TODO: Terminate this thread properly
            t1.join();
        }
    });

    t2.join();
    */

    // TODO: This currently simply infinite loops
    // startMarquee(outWindow, text, 10, max_x - 2);

    endwin();

    return 0;
}
