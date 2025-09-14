#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"
#include "../inc/marquee.h"

const int DEFAULT_FONT_HEIGHT = 6;
const int DEFAULT_REFRESH_DELAY = 100;

Marquee::Marquee (WINDOW *outWindow)
{
    this->outWindow = outWindow;
    this->refreshDelay = DEFAULT_REFRESH_DELAY;
    this->running = false;
    this->screenWidth = getmaxx(outWindow);
}

void Marquee::setRefreshDelay (int refreshDelay)
{
    {
        std::lock_guard<std::mutex> lock(mymutex);
        this->refreshDelay = refreshDelay;
        flag = true;
    }
    mycond.notify_all();
}

void Marquee::setText (std::string text)
{
    this->asciiText = convertToASCIIArt(text, this->screenWidth);   
}

void Marquee::stop ()
{
    this->running = false;
    wclear(this->outWindow);
    wrefresh(this->outWindow);
}

void Marquee::start ()
{
    if (this->asciiText.empty()) {
        // TODO: Do something when ASCII art is empty
        return;
    }

    const size_t rowCount = DEFAULT_FONT_HEIGHT;
    const size_t rowLen = this->asciiText[0].size();

    // Each element is a row; each character is a column
    // Initialized with whitespace to act as a blank canvas.
    std::vector<std::string> display(rowCount,
                                     std::string(this->screenWidth, ' '));

    size_t col = 0;

    this->running = true;

    // Loop for generating the marquee
    while (this->running) {

        std::unique_lock<std::mutex> lock(mymutex);

         /* Wait for either flag becomes true when setText gets called
            or refreshDelay to count down to 0  */
        mycond.wait_for( lock,
                        std::chrono::milliseconds(refreshDelay),
                        [this]() { return flag; } );

        // reset the flag
        flag = false;

        for (size_t row = 0; row < rowCount; row++) {
            // Remove the leftmost character from this row
            display[row].erase(0, 1);

            // Add the next character from the ASCII art reference
            display[row].push_back(asciiText[row][col]);

            mvwprintw(this->outWindow, row, 0, "%s", display[row].c_str());
        }

        wrefresh(this->outWindow);

        // Wrap once it reaches the end
        col = (col + 1) % rowLen;

    }
}
