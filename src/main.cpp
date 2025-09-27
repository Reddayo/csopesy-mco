#include <curses.h>
#include <string>

#include "../inc/display_manager.h"
#include "../inc/marquee.h"
#include "../inc/command_interpreter.h"


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
 

    CommandInterpreter ci(dm, marquee);

    ci.start();

    // End curses environment
    endwin();

    return 0;
}
