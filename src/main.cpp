#include <curses.h>
#include <string>
#include <thread>

#include "../inc/marquee.h"

/**
 * The height of the input window. The rest of the vertical space in the
 * terminal is allocated for the output window.
 */
const int INPUT_WINDOW_HEIGHT = 10;

// Terminal dimensions
int max_y, max_x;

// Borders for input and output
WINDOW *outBorder, *inputBorder;

// Borders for printing content
WINDOW *outWindow, *inputWindow;

/** Refreshes both the input and the output window */
void refreshWindows ()
{
    // TODO: Very redundant? I don't know'
    getmaxyx(stdscr, max_y, max_x);

    mvwin(inputBorder, max_y - INPUT_WINDOW_HEIGHT, 0);
    wresize(inputBorder, INPUT_WINDOW_HEIGHT, max_x);

    mvwin(inputWindow, max_y - INPUT_WINDOW_HEIGHT + 1, 1);
    wresize(inputWindow, INPUT_WINDOW_HEIGHT - 2, max_x - 2);

    wresize(outBorder, max_y - INPUT_WINDOW_HEIGHT, max_x);
    wresize(outWindow, max_y - INPUT_WINDOW_HEIGHT - 2, max_x - 2);

    // We only clear output window to preserve history in input window. This has
    // the side effect of causing the input window to appear glitchy.
    wclear(outWindow);
    clear();

    box(outBorder, 0, 0);
    box(inputBorder, 0, 0);

    mvwprintw(outBorder, 0, 2, " CSOPESY S12 ");
    mvwprintw(outBorder, 0, max_x - 38, " CESAR | LLOVIT | MARQUESES | SILVA ");

    wrefresh(outBorder);
    wrefresh(inputBorder);
    wrefresh(outWindow);
    wrefresh(inputWindow);
}

int main ()
{
    // Thread for marquee animation
    std::thread animThread;

    // =========================================================================

    initscr();            // Initialize the screen
    cbreak();             // Disable line buffering
    start_color();        // Allow for colors because colors are pretty
    keypad(stdscr, TRUE); // Enable special keys like KEY_RESIZE

    // Define color pairss
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_RED);

    // Get the dimensions of the terminal
    // WARNING: Display will break if you resize the terminal!
    getmaxyx(stdscr, max_y, max_x);

    // Create border windows
    outBorder = newwin(max_y - INPUT_WINDOW_HEIGHT, max_x, 0, 0);
    inputBorder =
        newwin(INPUT_WINDOW_HEIGHT, max_x, max_y - INPUT_WINDOW_HEIGHT, 0);

    // Draw the borders
    box(outBorder, 0, 0);
    box(inputBorder, 0, 0);

    // Content windows are created as subwindows of the border windows
    outWindow =
        subwin(outBorder, max_y - INPUT_WINDOW_HEIGHT - 2, max_x - 2, 1, 1);

    inputWindow = subwin(inputBorder, INPUT_WINDOW_HEIGHT - 2, max_x - 2,
                         max_y - INPUT_WINDOW_HEIGHT + 1, 1);

    mvwprintw(outBorder, 0, 2, " CSOPESY S12 ");
    mvwprintw(outBorder, 0, max_x - 38, " CESAR | LLOVIT | MARQUESES | SILVA ");
    
    // Refresh all windows
    wrefresh(outBorder);
    wrefresh(inputBorder);
    wrefresh(outWindow);
    wrefresh(inputWindow);

    // Enable scrolling for input window
    scrollok(inputWindow, true);
    idlok(inputWindow, true);

    // Initialize the marquee
    Marquee marquee(outWindow);

    // =========================================================================

    // Main loop for fetching input
    // curses requires a C-style char buffer to hold the thing
    char buffer[100] = "";

    bool show_prompt = true;

    while (1) {
        if (show_prompt) {
            wattron(inputWindow, COLOR_PAIR(1));
            wprintw(inputWindow, " Command ");
            wattroff(inputWindow, COLOR_PAIR(1));
            wprintw(inputWindow, " > ");
        }

        echo();

        // If wgetstr() is interrupted by a SIGWINCH signal (terminal was
        // resized), it returns KEY_RESIZE
        if (wgetstr(inputWindow, const_cast<char *>(buffer)) == KEY_RESIZE) {
            refreshWindows();
            show_prompt = false; // Don't print a prompt on the next iteration
            continue;            // Skip parsing
        }

        show_prompt = true;

        // Convert the command input buffer into a C++ string
        std::string command(buffer);

        // Parse command
        // TODO: Help and pause commands
        if (command == "start_marquee") {
            // marquee.stop() will end the infinite loop in marquee.start()

            // WARNING: This is done so that the thread can properly DETACH
            // itself and die before we create a new thread
            marquee.stop();
            animThread = std::thread([&marquee, &animThread] () {
                marquee.start();
                animThread.detach();
            });
        } else if (command == "stop_marquee") {
            marquee.stop();
        } else if (command == "exit") {
            marquee.stop();
            break;
        } else if (command.rfind("set_text ", 0) == 0) {
            marquee.setText(command.substr(9));
        } else if (command.rfind("set_speed ", 0) == 0) {
            // TODO: Can we bypass std::this_thread::sleep_for()?
            marquee.setRefreshDelay(std::stoi(command.substr(10)));
        } else if (command == "refresh") {
            refreshWindows();
        } else {
            wattron(inputWindow, COLOR_PAIR(2));
            wprintw(inputWindow, " Error   ");
            wattroff(inputWindow, COLOR_PAIR(2));
            wprintw(inputWindow, " Unknown command.\n");
        }
    }

    // End curses environment
    endwin();

    return 0;
}
