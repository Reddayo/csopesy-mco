#include <curses.h>
#include <filesystem>
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

    CommandInterpreter ci_process(dm);

    CommandInterpreter ci_main(dm);

    // System can run without config.txt, and user can only exit ---------------

    if (!std::filesystem::exists("config.txt")) {
        dm.showErrorPrompt("File config.txt does not exist in the directory. "
                           "Use command \"exit\" to close program.");

        ci_main.addCommand("exit", 0, false, //
                           [&dm, &ci_main] (CommandArguments &) {
                               ci_main.exitInputs();
                               endwin();
                           });

        ci_main.startInputs();
    }

    // =========================================================================

    Config config("config.txt");


    // Could be better
    MemoryManager mm("csopesy-backing-store.txt", config.getMaxOverAllMem(), config.getMemPerFrame());

    OS os(dm, mm, config);

    // Main command interpreter ------------------------------------------------

    // help command. Argument list goes unused
    ci_main.addCommand(
        "help", 0, false, //
        [&dm] (CommandArguments &) {
            dm.clearOutputWindow();
            dm._mvwprintw(
                1, 0, "%s",
                "Main menu commands:\n"
                "initialize                 - initialize the processor "
                "configuration of the application\n"
                "scheduler-start            - continuously generate dummy "
                "processes for the CPU scheduler\n"
                "scheduler-stop             - stop generating dummy processes\n"
                "screen -s <process name>   - create a new process and access "
                "its screen\n"
                "screen -r <process name>   - access the screen of an existing "
                "process\n"
                "screen -ls                 - list all running processes\n"
                "report-util                - generate CPU utilization report "
                "in csopesy-log.txt\n"
                "exit                       - terminate the console");
        });

    // Initialize OS. Must be called before anything else
    ci_main.addCommand(
        "initialize", 0, false, //
        [&os, &config, &dm, &ci_main, &ci_process] (CommandArguments &) {
            if (os.isRunning()) {
                os.exit();
                os.reset();
                config = Config("config.txt");
                os.updateConfig(config);
                os.run();
            } else
                os.run();
        });

    // Start randomly generating dummy processes
    ci_main.addCommand(
        "scheduler-start", 0, false, //
        [&os, &dm] (CommandArguments &) {
            if (!os.isRunning())
                dm.showErrorPrompt(
                    "OS is not initialized. Use command \"initialize\" first.");
            else if (!os.isGenerating())
                os.setGenerateDummyProcesses(true);
            else
                dm.showErrorPrompt("Scheduler is already started.");
        });

    // Same command as scheduler-start according to specs
    ci_main.addCommand(
        "scheduler-test", 0, false, //
        [&os, &dm] (CommandArguments &) {
            if (!os.isRunning())
                dm.showErrorPrompt(
                    "OS is not initialized. Use command \"initialize\" first.");
            else if (!os.isGenerating())
                os.setGenerateDummyProcesses(true);
            else
                dm.showErrorPrompt("Scheduler is already started.");
        });

    // Stop randomly generating dummy processes
    ci_main.addCommand(
        "scheduler-stop", 0, false, //
        [&os, &dm] (CommandArguments &) {
            if (!os.isRunning())
                dm.showErrorPrompt(
                    "OS is not initialized. Use command \"initialize\" first.");
            else if (os.isGenerating())
                os.setGenerateDummyProcesses(false);
            else
                dm.showErrorPrompt("Scheduler is not yet started.");
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
                os.ls(false);
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
            } /* else if(args[0].substr(0, 3) == "-c ") {
                dm.clearInputWindow();
                dm.clearOutputWIndow();

                might be ideal to pre process memsize here, instead of in the OS, 
                so showErrorPrompt  can be used
                os.screenC(args[0].subtr(3), and the "instruction_string here");
            }*/ else {
                dm.showErrorPrompt("Invalid command");
                return;
            }

            dm.setOutputBorderLabel(args[0].substr(3));

            // Pass control to process command interpreter
            ci_main.exitInputs();
            ci_process.startInputs();
        });

    // report-util command. Same as screen -ls but writes to a file instead
    ci_main.addCommand( //
        "report-util", 0, false, [&os] (CommandArguments &) { os.ls(true); });

    ci_main.addCommand( //
        "process-smi", 0, false,
        [&os] (CommandArguments &) { os.processSMI_main(); });
    
    // vmstat command
    ci_main.addCommand( //
        "vmstat", 0, false,
        [&os] (CommandArguments &) { os.vmstat(); });

    // Process command interpreter ---------------------------------------------

    // help command. Argument list goes unused
    ci_process.addCommand(
        "help", 0, false, //
        [&dm] (CommandArguments &) {
            dm.clearOutputWindow();
            dm._mvwprintw(
                1, 0, "%s",
                "Screen commands:\n"
                "process-smi                - print updated details and logs "
                "from print instructions\n"
                "exit                       - return to the main menu");
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

    ci_process.addCommand( //
        "process-smi", 0, false,
        [&os] (CommandArguments &) { os.processSMI_process(); });

    // =========================================================================

    // Start collecting inputs from the user
    ci_main.startInputs();

    // End curses environment
    endwin();

    return 0;
}
