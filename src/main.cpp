#include <curses.h>
#include <map>
#include <string>
#include <thread>

#include "../inc/command_interpreter.h"
#include "../inc/config.h"
#include "../inc/display_manager.h"
#include "../inc/os.h"

int main ()
{
    initscr();            // Initialize the screen
    cbreak();             // Disable line buffering
    start_color();        // Allow for colors because colors are pretty
    keypad(stdscr, TRUE); // Enable special keys like KEY_RESIZE
    noecho();             // Block input echo (we need to do it manually)

    // Define color pairs
    init_pair(1, COLOR_BLACK, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);

    // Initialize display manager
    DisplayManager dm = DisplayManager();

    // Show title screen
    dm.showTitleScreen();

    // =========================================================================

    Config config("config.txt");

    OS os(dm, config);

    // =========================================================================

    CommandInterpreter ci(dm);

    // help command. Argument list goes unused
    ci.addCommand("help", 0, false, [&dm] (CommandArguments &) {
        // Pause the animation, clear the output window, and show help.
        dm.clearOutputWindow();
        dm._mvwprintw(0, 0, "%s", "You called for help, but nobody came");
    });

    ci.addCommand("initialize", 0, false,
                  [&os] (CommandArguments &) { os.run(); });

    ci.addCommand("ls", 0, false, [&os] (CommandArguments &) { os.ls(); });

    // =========================================================================

    // Start collecting inputs from the user
    ci.startInputs();

    // End curses environment
    endwin();

    return 0;
}
