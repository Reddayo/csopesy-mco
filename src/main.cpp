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

    CommandInterpreter ci_process(dm);

    CommandInterpreter ci_main(dm);

    // Main command interpreter

    // help command. Argument list goes unused
    ci_main.addCommand("help", 0, false, [&dm] (CommandArguments &) {
        // Pause the animation, clear the output window, and show help.
        dm.clearOutputWindow();
        dm._mvwprintw(0, 0, "%s", "You called for help, but nobody came");
    });

    ci_main.addCommand("initialize", 0, false,
                       [&os] (CommandArguments &) { os.run(); });

    ci_main.addCommand("scheduler-start", 0, false, [&os] (CommandArguments &) {
        os.setGenerateDummyProcesses(true);
    });

    ci_main.addCommand("scheduler-stop", 0, false, [&os] (CommandArguments &) {
        os.setGenerateDummyProcesses(false);
    });

    ci_main.addCommand("exit", 0, false, [&os, &ci_main] (CommandArguments &) {
        os.exit();
        ci_main.exitInputs();
    });

    /*
    // TODO
    ci_main.addCommand(
        // Switch to process screen
        "screen", 0, false,
        [&os, &dm, &ci_main, &ci_process] (CommandArguments &) {
            dm.clearInputWindow();
            dm.clearOutputWindow();

            // TODO: Do nothing idk

            ci_main.exitInputs();
            ci_process.startInputs();
        });
    */

    ci_main.addCommand(
        // Switch to process screen
        "screen", 1, true,
        [&os, &dm, &ci_main, &ci_process] (CommandArguments &args) {
            // War crimes ahead
            bool valid = true;

            if (args[0] == "-ls")
                os.ls();
            else if (args[0].substr(0, 3) == "-s ") {
                dm.clearInputWindow();
                dm.clearOutputWindow();
                dm._mvwprintw(0, 0, "%s", "-s works");
            } else if (args[0].substr(0, 3) == "-r ") {
                dm.clearInputWindow();
                dm.clearOutputWindow();
                dm._mvwprintw(0, 0, "%s", "-r works");
            } else {
                dm.showErrorPrompt("Invalid command");
                valid = false;
            }

            if (valid) {
                ci_main.exitInputs();
                ci_process.startInputs();
            }
        });

    // Process command interpreter

    ci_process.addCommand(
        // Switch to main menu screen
        "exit", 0, false,
        [&os, &dm, &ci_main, &ci_process] (CommandArguments &) {
            dm.clearInputWindow();
            dm.clearOutputWindow();

            dm.showTitleScreen();

            ci_process.exitInputs();
            ci_main.startInputs();
        });

    // =========================================================================

    // Start collecting inputs from the user
    ci_main.startInputs();

    // End curses environment
    endwin();

    return 0;
}
