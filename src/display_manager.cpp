#include <curses.h>
#include <mutex>

#include "../inc/display_manager.h"

/**
 * The height of the input window. The rest of the vertical space in the
 * terminal is allocated for the output window.
 */
const int INPUT_WINDOW_HEIGHT = 10;

const std::vector<std::string> titleScreenArt = {
    "  _   _                        _  ___  ___  ",
    " | |_(@)___ __ _ _ _ __ _ __ _(@)/ _ \\/ __| ",
    " | / | |_-</ _` | '_/ _` / _` | | (_) \\__ \\ ",
    " |_\\_|_|__/\\__,_|_| \\__,_\\__, |_|\\___/|___/ ",
    "                         |___/              ",
    "                                            ",
    "    o_,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_o     ",
    "                                            ",
    "       - CSOPESY MCO: OS Emulator -         ",
    "                                            ",
    "        CESAR, Jusper Angelo Manuel         ",
    "        LLOVIT, Benn Erico Bernardo         ",
    "     MARQUESES, Lorenz Bernard Baduria      ",
    "    SILVA, Paulo Grane Gabriel Carcamo      ",
    "                                            ",
    "    o_,.-'~'-.,__,.-'~'-.,__,.-'~'-.,_o     ",
    "                                            ",
    "    Type \"help\" for a list of commands.      "};

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
    // Lock the display manager
    std::lock_guard<std::mutex> lock(this->mutex);

    wrefresh(this->outputBorder);
    wrefresh(this->inputBorder);
    wrefresh(this->outputWindow);
    wrefresh(this->inputWindow);
}

void DisplayManager::showInputPrompt ()
{
    // Lock the display manager
    std::lock_guard<std::mutex> lock(this->mutex);

    wattron(this->inputWindow, COLOR_PAIR(1));
    wprintw(this->inputWindow, " Command ");
    wattroff(this->inputWindow, COLOR_PAIR(1));
    wprintw(this->inputWindow, " > ");
}

void DisplayManager::showErrorPrompt (std::string message)
{
    // Lock the display manager
    std::lock_guard<std::mutex> lock(this->mutex);

    wattron(inputWindow, COLOR_PAIR(2));
    wprintw(inputWindow, " Error   ");
    wattroff(inputWindow, COLOR_PAIR(2));
    wprintw(inputWindow, " %s\n", message.c_str());
}

void DisplayManager::showTitleScreen ()
{
    int x_start = this->getWindowWidth() / 2 - 22;
    int y_start = this->getOutputWindowHeight() / 2 - 9;
    int y_index = 0;

    // Switch to blue color
    wattron(this->outputWindow, COLOR_PAIR(3));

    for (std::string row : titleScreenArt) {
        this->_mvwprintw(y_start + y_index, x_start, "%s", row.c_str());

        // Switch to normal color after printing the logo
        if (y_index > 4)
            wattroff(this->outputWindow, COLOR_PAIR(3));

        y_index++;
    }
}

int DisplayManager::getWindowWidth () { return getmaxx(this->outputWindow); }

int DisplayManager::getOutputWindowHeight ()
{
    return getmaxy(this->outputWindow);
}

void DisplayManager::clearOutputWindow ()
{
    // Lock the display manager
    std::lock_guard<std::mutex> lock(this->mutex);

    wclear(this->outputWindow);
    wrefresh(this->outputWindow);
}

int DisplayManager::_mvwprintw (int y, int x, const char *format, ...)
{
    int status;
    va_list varglist;

    // Lock the display manager
    std::lock_guard<std::mutex> lock(this->mutex);

    // Fetch variadic arguments
    va_start(varglist, format);

    // Move cursor and print to that position
    wmove(this->outputWindow, y, x);
    status = vw_printw(this->outputWindow, format, varglist);

    // Automatically refresh
    wrefresh(this->outputWindow);

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
    std::unique_lock<std::mutex> lock(this->mutex);
    ch = wgetch(this->inputWindow);
    lock.unlock();

    // Unset non-blocking mode
    nodelay(this->inputWindow, FALSE);

    // wgetch() will have returned ERR if there was no input
    if (ch == ERR) {
        return ERR;
    }

    // Save the current position of the cursor (needed so we can move backwards)
    getyx(this->inputWindow, y, x);

    // Handle backspace key
    if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
        if (currentLength > 0) {
            // Remove the last character from the buffer
            currentLength--;
            buffer[currentLength] = '\0';

            lock.lock();

            // Adjust cursor in the input window
            // Handle if the cursor went down a line to fit the whole command
            if (x > 0) {
                // Go back to the left by one character
                mvwaddch(this->inputWindow, y, x - 1, ' ');
                wmove(this->inputWindow, y, x - 1);
            } else {
                // Go back to the end of the previous line
                mvwaddch(this->inputWindow, y - 1, this->getWindowWidth() - 1,
                         ' ');
                wmove(this->inputWindow, y - 1, this->getWindowWidth() - 1);
            }

            lock.unlock();
        }

        return INPUT_READ_THROUGH;
    }

    // Handle newline or carriage return
    else if (ch == '\n' || ch == '\r') {
        buffer[currentLength] = '\0';
        currentLength = 0;

        lock.lock();

        waddch(this->inputWindow, '\n');
        wrefresh(this->inputWindow);

        lock.unlock();

        return INPUT_READ_SUBMIT;
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

        return INPUT_READ_THROUGH;
    }

    return INPUT_READ_BUFFER_FULL;
}
