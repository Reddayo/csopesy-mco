#include <curses.h>
#include <string>

#include "../inc/display_manager.h"
#include "../inc/marquee.h"

// Terminal dimensions
int max_y, max_x;

int main ()
{
    initscr();            // Initialize the screen
    cbreak();             // Disable line buffering
    start_color();        // Allow for colors because colors are pretty
    keypad(stdscr, TRUE); // Enable special keys like KEY_RESIZE
    noecho();

    // Define color pairss
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    // Initialize display manager
    DisplayManager dm = DisplayManager();

    // Initialize the marquee animation
    Marquee marquee(dm);

    // =========================================================================

    // Main loop for fetching input
    // curses requires a C-style char buffer to hold the thing
    char buffer[100] = "";
    size_t size = 0;

    bool show_prompt = true;

    while (1) {
        if (show_prompt) {
            dm.showInputPrompt();
        }

        int read = dm._wgetnstr(buffer, 100, size);

        if (read != 1) {
            show_prompt = false;

            continue;
        }

        show_prompt = true;

        // Convert the command input buffer into a C++ string
        std::string command(buffer);

        // Parse command
        // TODO: Help and pause commands
        if (command == "start_marquee") {
            // WARNING: This is done so that the thread can properly JOIN with
            // the main thread and die before we create a new thread
            marquee.stop();
            marquee.start();
        } else if (command == "stop_marquee") {
            marquee.stop();
        } else if (command == "exit") {
            marquee.stop();
            break;
        } else if (command.rfind("set_text ", 0) == 0) {
            marquee.setText(command.substr(9));
        } else if (command.rfind("set_speed ", 0) == 0) {
            marquee.setRefreshDelay(std::stoi(command.substr(10)));
        } else if (command == "refresh") {
            // TODO: Cleaner way to refresh windows
            dm.refreshAll();
        } else {
            dm.showErrorPrompt();
        }
    }

    // End curses environment
    endwin();

    return 0;
}
