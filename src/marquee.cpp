#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"
#include "../inc/display_manager.h"
#include "../inc/marquee.h"

const int DEFAULT_FONT_HEIGHT = 7;
const int DEFAULT_REFRESH_DELAY = 100;

Marquee::Marquee (DisplayManager &dm) : dm(dm)
{
    this->refreshDelay = DEFAULT_REFRESH_DELAY;
    this->animationRunning = false;
    this->screenWidth = dm.getWindowWidth();
}

void Marquee::setText (std::string text)
{
    this->asciiText = convertToASCIIArt(text, this->screenWidth);

    // Force the thread to wake up
    animationInterrupted = true;
    wakeUp.notify_all();
}

void Marquee::setRefreshDelay (int refreshDelay)
{
    std::lock_guard<std::mutex> lock(mutex);

    this->refreshDelay = refreshDelay;

    // Force the thread to wake up
    animationInterrupted = true;
    wakeUp.notify_all();
}

void Marquee::stop ()
{
    this->animationRunning = false;

    // Force the thread to wake up
    animationInterrupted = true;
    wakeUp.notify_all();

    if (this->animThread.joinable()) {
        this->animThread.join();
    }
}

void Marquee::start ()
{
    if (this->asciiText.empty()) {
        // TODO: Do something when ASCII art is empty
        dm.showErrorPrompt("No marquee text set!");
        return;
    }

    if (this->animationRunning) {
        dm.showErrorPrompt("Marquee animation already running!");
        return;
    }

    dm.clearOutputWindow();

    const size_t rowCount = DEFAULT_FONT_HEIGHT;

    this->animationRunning = true;

    // Avoid orphaning the current animation thread, if it already exists
    if (this->animThread.joinable()) {
        this->animThread.join();
    }

    this->animThread = std::thread([this] () {
        // Loop for generating the marquee
        while (this->animationRunning) {
            // Locks the marquee thread
            std::unique_lock<std::mutex> lock(mutex);

            // Thread will sleep for refreshDelay or when interrupted
            wakeUp.wait_for(lock, std::chrono::milliseconds(refreshDelay),
                            [this] () { return animationInterrupted; });

            // Always reset the animationInterrupted flag
            animationInterrupted = false;

            for (size_t row = 0; row < rowCount; row++) {
                // Cycle the ASCII art
                asciiText[row].push_back(asciiText[row][0]);
                asciiText[row].erase(0, 1);

                dm._mvwprintw(row, 0, "%.*s", this->screenWidth,
                              asciiText[row].c_str());
            }
        }
    });
}
