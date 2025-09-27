#include <curses.h>
#include <string>

#include "../inc/display_manager.h"
#include "../inc/marquee.h"

int main ()
{
    initscr();            // Initialize the screen
    cbreak();             // Disable line buffering
    start_color();        // Allow for colors because colors are pretty
    keypad(stdscr, TRUE); // Enable special keys like KEY_RESIZE
    noecho();             // Block input echo (we need to do it manually)

    // Terminal dimensions
    int max_y, max_x;

    // Define color pairss
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    // Initialize display manager
    DisplayManager dm = DisplayManager();

    // Initialize the marquee animation
    Marquee marquee(dm);

    // =========================================================================
    // Main loop for fetching input

    // C-style char buffer to hold user input
    char buffer[100] = "";

    // Holds the current size of the input buffer
    size_t size = 0;

    int read;

    dm.showInputPrompt();

    while (1) {
        read = dm._wgetnstr(buffer, 100, size);

        if (read != INPUT_READ_SUBMIT) {
            // Don't process the contents of the input buffer until user submits
            continue;
        }

        // Convert the command input buffer into a C++ string
        std::string command(buffer);

        // Parse command
        // TODO: Help command
        if (command == "start_marquee") {
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
        } else if (command == "help") {
            // Pause the animation, clear the output window, and show help.
            marquee.stop();
            dm.clearOutputWindow();
            dm._mvwprintw(
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
        } else {
            dm.showErrorPrompt("Unknown command");
        }

        // Show the prompt for the next input
        dm.showInputPrompt();
    }

    // End curses environment
    endwin();

    return 0;
}
