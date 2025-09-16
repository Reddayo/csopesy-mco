#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"
#include "../inc/marquee.h"

const int DEFAULT_FONT_HEIGHT = 7;
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

    if (this->animThread.joinable()) {
        this->animThread.join();
    }

    wrefresh(this->outWindow);
}

void Marquee::help ()
{
    this->running = false;
    
    if (this->animThread.joinable()) {
        this->animThread.join();
    }

    this->animThread = std::thread([this] () {
        mvwprintw(this->outWindow, 0, 0, "%s",
        "help           - displays the commands and its description\n"
        "start_marquee  - starts the marquee animation\n"
        "stop_marquee   - stops the marquee animation\n"
        "set_text       - accepts a text input and displays it as a marquee\n"
        "set_speed      - sets the marquee animation refresh in milliseconds\n"
        "exit           - terminates the console\n"
        "refresh        - refresh the windows");

        wrefresh(this->outWindow);
    });
}

void Marquee::start ()
{
    if (this->asciiText.empty()) {
        // TODO: Do something when ASCII art is empty
        return;
    }

    const size_t rowCount = DEFAULT_FONT_HEIGHT;

    this->running = true;

    // WARNING: Does not join the existing animThread before intantiating a new
    // thread. Call Marquee::stop() first!
    this->animThread = std::thread([this] () {
        // Loop for generating the marquee
        while (this->running) {

            std::unique_lock<std::mutex> lock(mymutex);

            /* Wait for either flag becomes true when setText gets called
             *      or refreshWindowsDelay to count down to 0  */
            mycond.wait_for(lock, std::chrono::milliseconds(refreshDelay),
                            [this] () { return flag; });

            // Reset the flag
            flag = false;

            for (size_t row = 0; row < rowCount; row++) {
                // Cycle the ASCII art
                asciiText[row].push_back(asciiText[row][0]);
                asciiText[row].erase(0, 1);

                // Print up to window size
                mvwprintw(this->outWindow, row, 0, "%.*s", this->screenWidth,
                          asciiText[row].c_str());
            }

            wrefresh(this->outWindow);
        }
    });
}
