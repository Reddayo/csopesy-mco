#include <curses.h>

#include "../inc/marquee.h"

const int INPUT_WINDOW_HEIGHT = 10;

int main ()
{
    int max_y, max_x;

    initscr(); // Initialize the screen
    cbreak();  // Disable line buffering
    noecho();  // Disable input echo

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

    std::string text = "SHUPO SHUPO SHUPO SHUPO SHUPO SHUPO SHUPO SHUPO SHUPO "
                       "SHUPO SHUPO SHUPO SHUPO SHUPO SHUPO SHUPO ";

    // TODO: This currently simply infinite loops
    startMarquee(outWindow, text, 10, max_x - 2);

    endwin();

    return 0;
}