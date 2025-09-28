#include <curses.h>
#include <stdexcept>
#include <string>

#include "../inc/command_interpreter.h"
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

    CommandInterpreter ci(dm);

    // start_marquee command. Argument list goes unused
    ci.addCommand("start_marquee", // Name
                  0,               // Parameter count
                  false,           // Allow whitespace in argument
                  [&marquee] (CommandArguments &) { marquee.start(); });

    // stop_marquee command. Argument list goes unused
    ci.addCommand("stop_marquee", 0, false,
                  [&marquee] (CommandArguments &) { marquee.stop(); });

    // refresh command. Argument list goes unused
    ci.addCommand("refresh", 0, false,
                  [&dm] (CommandArguments &) { dm.refreshAll(); });

    // help command. Argument list goes unused
    ci.addCommand("help", 0, false, [&dm, &marquee] (CommandArguments &) {
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
    });

    // exit command. Argument list goes unused
    ci.addCommand("exit", 0, false, [&ci, &marquee] (CommandArguments &) {
        marquee.stop();
        ci.exitInputs(); // Exits the command interpreter loop
    });

    // set_text command
    ci.addCommand("set_text", 1, true, [&marquee] (CommandArguments &args) {
        marquee.setText(args[0]);
    });

    // set_speed command
    ci.addCommand("set_speed", 1, false, [&marquee] (CommandArguments &args) {
        int speed;

        // Handle invalid argument type
        try {
            speed = std::stoi(args[0]);
        } catch (const std::invalid_argument &e) {
            throw std::invalid_argument("Argument must be numeric");
        }

        // Handle negative speed values
        if (speed < 0) {
            throw std::out_of_range("Value cannot be negative.");
        }

        marquee.setRefreshDelay(speed);
    });

    // =========================================================================

    // Start collecting inputs from the user
    ci.startInputs();

    // End curses environment
    endwin();

    return 0;
}
