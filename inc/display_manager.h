#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <cstdarg>
#include <curses.h>
#include <functional>
#include <mutex>
#include <queue>

#define INPUT_READ_THROUGH 0
#define INPUT_READ_SUBMIT 1
#define INPUT_READ_BUFFER_FULL -2

/**
 * Class that manages program display, including the input and output windows.
 */
class DisplayManager
{
  public:
    DisplayManager();

    /** Refreshes the input and output windows and their borders */
    void refreshAll();

    /** @return The windows' width */
    int getWindowWidth();

    /** Prints an input prompt to the input window */
    void showInputPrompt();

    /** Prints an error prompt in the input window */
    void showErrorPrompt();

    /**
     * Thread-safe wrapper function for curses' mvwprintw().
     *
     * @param y Moves the cursor to this y coordinate
     * @param x Moves the cursor to this x coordinate
     * @param format Format string to print. Followed by a variable list of
     * arguments.
     *
     * @return ERR on failure or OK on success
     */
    int _mvwprintw(int y, int x, const char *fmt, ...);

    /**
     * Thread-safe (non-blocking) function that emulates the functionality of
     * ncurses' wgetnstr(). Accepts a single character as user input and
     * increments a counter up to a maximum input length.
     *
     * @param buf C-style string buffer to write the user input to
     * @param maxLength The maximum length of the user input
     * @param currentLength The current length of the buffer
     *
     * @return INPUT_READ_THROUGH for accepting normal characters,
     * INPUT_READ_SUBMIT when user submits with a newline,
     * INPUT_READ_BUFFER_FULL if buffer is full, and ERR if wgetch() call fails
     */
    int _wgetnstr(char *buf, size_t maxLength, size_t &currentLength);

  private:
    WINDOW *outputBorder, *inputBorder;

    WINDOW *outputWindow, *inputWindow;

    std::mutex mutex;
};

#endif
