#include <curses.h>
#include <mutex>

#include "../inc/display_manager.h"

/**
 * The height of the input window. The rest of the vertical space in the
 * terminal is allocated for the output window.
 */
const int INPUT_WINDOW_HEIGHT = 10;

DisplayManager::DisplayManager ()
{
    int max_x, max_y;

    // Get the dimensions of the terminal
    // WARNING: Display will break if you resize the terminal!
    getmaxyx(stdscr, max_y, max_x);

    // Create border windows
    this->outputBorder = newwin(max_y - INPUT_WINDOW_HEIGHT, max_x, 0, 0);
    this->inputBorder =
        newwin(INPUT_WINDOW_HEIGHT, max_x, max_y - INPUT_WINDOW_HEIGHT, 0);

    // Draw the borders
    box(outputBorder, 0, 0);
    box(inputBorder, 0, 0);

    // Content windows are created as subwindows of the border windows
    this->outputWindow =
        subwin(outputBorder, max_y - INPUT_WINDOW_HEIGHT - 2, max_x - 2, 1, 1);
    this->inputWindow = subwin(inputBorder, INPUT_WINDOW_HEIGHT - 2, max_x - 2,
                               max_y - INPUT_WINDOW_HEIGHT + 1, 1);

    // Print border titles
    mvwprintw(outputBorder, 0, 2, " CSOPESY S12 ");
    mvwprintw(outputBorder, 0, max_x - 38,
              " CESAR | LLOVIT | MARQUESES | SILVA ");

    this->refreshAll();

    // Enable scrolling for input window
    scrollok(inputWindow, true);
    idlok(inputWindow, true);
}

void DisplayManager::refreshAll ()
{
    std::unique_lock<std::mutex> lock(this->queueMtx);

    wrefresh(this->outputBorder);
    wrefresh(this->inputBorder);
    wrefresh(this->outputWindow);
    wrefresh(this->inputWindow);

    lock.unlock();
}

void DisplayManager::showInputPrompt ()
{
    std::unique_lock<std::mutex> lock(this->queueMtx);

    wattron(this->inputWindow, COLOR_PAIR(1));
    wprintw(this->inputWindow, " Command ");
    wattroff(this->inputWindow, COLOR_PAIR(1));
    wprintw(this->inputWindow, " > ");

    lock.unlock();
}

void DisplayManager::showErrorPrompt ()
{
    std::unique_lock<std::mutex> lock(this->queueMtx);

    wattron(inputWindow, COLOR_PAIR(2));
    wprintw(inputWindow, " Error   ");
    wattroff(inputWindow, COLOR_PAIR(2));
    wprintw(inputWindow, " Unknown command.\n");

    lock.unlock();
}

int DisplayManager::getWindowWidth () { return getmaxx(this->outputWindow); }

int DisplayManager::_mvwprintw (int y, int x, const char *format, ...)
{
    int status;

    std::unique_lock<std::mutex> lock(this->queueMtx);

    va_list varglist;
    va_start(varglist, format);
    wmove(this->outputWindow, y, x);
    status = vw_printw(this->outputWindow, format, varglist);

    lock.unlock();

    return status;
}

// TODO: Handle the screen resize signal maybe
int DisplayManager::_wgetnstr (char *buffer,
                               size_t maxLength,
                               size_t &currentLength)
{
    int ch, y, x;

    // Set the window to non-blocking so wgetch isn't forced to wait for input
    nodelay(this->inputWindow, TRUE);

    // Lock and get a single character input
    std::unique_lock<std::mutex> lock(this->queueMtx);
    ch = wgetch(this->inputWindow);
    lock.unlock();

    // Unset non-blocking mode
    nodelay(this->inputWindow, FALSE);

    // wgetch() will have returned ERR if there was no input
    if (ch == ERR) {
        return -1;
    }

    // Save the current position of the cursor (needed so we can move backwards)
    getyx(this->inputWindow, y, x);

    // Handle backspace key
    if (ch == KEY_BACKSPACE || ch == 127) {
        if (currentLength > 0) {
            // Remove the last character from the buffer
            currentLength--;
            buffer[currentLength] = '\0';

            // Adjust cursor in the input window
            lock.lock();

            // Handle if the cursor went down a line to fit the whole command
            if (x > 0) {
                mvwaddch(this->inputWindow, y, x - 1, ' ');
                wmove(this->inputWindow, y, x - 1);
            } else {
                mvwaddch(this->inputWindow, y - 1, this->getWindowWidth() - 1,
                         ' ');
                wmove(this->inputWindow, y - 1, this->getWindowWidth() - 1);
            }

            lock.unlock();
        }

        return 0;
    }

    // Handle newline or carriage return
    else if (ch == '\n' || ch == '\r') {
        buffer[currentLength] = '\0';
        currentLength = 0;

        lock.lock();

        waddch(this->inputWindow, '\n');
        wrefresh(this->inputWindow);

        lock.unlock();

        return 1;
    }

    // Handle regular characters
    else if (currentLength < maxLength - 1) {
        buffer[currentLength] = ch;
        currentLength++;
        buffer[currentLength] = '\0';

        lock.lock();

        waddch(this->inputWindow, ch);
        wrefresh(this->inputWindow);

        lock.unlock();

        return 0;
    }

    return -2; // Buffer full
}
