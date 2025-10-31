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
    ci_main.addCommand(
        "help", 0, false, //
        [&dm] (CommandArguments &) {
            dm.clearOutputWindow();
            dm._mvwprintw(0, 0, "%s", " You called for help, but nobody came");
        });

    // Initialize OS. Must be called before anything else
    ci_main.addCommand(
        "initialize", 0, false, //
        [&os, &dm, &ci_main, &ci_process] (CommandArguments &) { os.run(); });

    // Start randomly generating dummy processes
    ci_main.addCommand(
        "scheduler-start", 0, false, //
        [&os, &dm] (CommandArguments &) {
            if (!os.isRunning())
                dm.showErrorPrompt(
                    "OS is not initialized. Use command \"initialize\" first.");
            else
                os.setGenerateDummyProcesses(true);
        });

    // Stop randomly generating dummy processes
    ci_main.addCommand(
        "scheduler-stop", 0, false, //
        [&os, &dm] (CommandArguments &) {
            if (!os.isRunning())
                dm.showErrorPrompt(
                    "OS is not initialized. Use command \"initialize\" first.");
            else
                os.setGenerateDummyProcesses(false);
        });

    // Exit the PROGRAM
    ci_main.addCommand(   //
        "exit", 0, false, //
        [&os, &dm, &ci_main] (CommandArguments &) {
            os.exit();
            ci_main.exitInputs();
        });

    // screen command
    // screen -s and -r will cause a switch to process screen, but not -ls
    ci_main.addCommand(
        "screen", 1, true,
        [&os, &dm, &ci_main, &ci_process] (CommandArguments &args) {
            if (!os.isRunning()) {
                dm.showErrorPrompt(
                    "OS is not initialized. Use command \"initialize\" first.");
                // Early return if OS is not running yet
                return;
            }

            // screen -ls
            if (args[0] == "-ls") {
                os.ls();
                return;
            } // screen -s process_name
            else if (args[0].substr(0, 3) == "-s ") {
                dm.clearInputWindow();
                dm.clearOutputWindow();
                os.screenS(args[0].substr(3));
            } // screen -r process_name
            else if (args[0].substr(0, 3) == "-r ") {
                dm.clearInputWindow();
                dm.clearOutputWindow();
                os.screenR(args[0].substr(3));
            } else {
                dm.showErrorPrompt("Invalid command");
                return;
            }

            dm.setOutputBorderLabel(args[0].substr(3));

            // Pass control to process command interpreter
            ci_main.exitInputs();
            ci_process.startInputs();
        });

    // Process command interpreter

    // help command. Argument list goes unused
    ci_process.addCommand(
        "help", 0, false, //
        [&dm] (CommandArguments &) {
            dm.clearOutputWindow();
            dm._mvwprintw(0, 0, "%s", " You called for help, but nobody came");
        });

    // Exit to MAIN SCREEN
    ci_process.addCommand(
        "exit", 0, false,
        [&os, &dm, &ci_main, &ci_process] (CommandArguments &) {
            dm.clearInputWindow();
            dm.clearOutputWindow();

            dm.showTitleScreen();

            dm.clearOutputBorderLabel();

            ci_process.exitInputs();
            ci_main.startInputs();
        });

    // TODO: process-smi
    ci_process.addCommand( //
        "process-smi", 0, false,
        [&os] (CommandArguments &) { os.processSMI(); });

    // =========================================================================

    // Start collecting inputs from the user
    ci_main.startInputs();

    // End curses environment
    endwin();

    return 0;
}
