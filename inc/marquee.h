#ifndef MARQUEE_H
#define MARQUEE_H

#include <condition_variable>
#include <curses.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../inc/display_manager.h"

class Marquee
{
  public:
    /**
     * Creates a new marquee instance.
     *
     * @param refreshDelay  The delay between each animation frame in ms
     */
    Marquee(DisplayManager &dm);

    /** Begins the marquee animation. */
    void start();

    /** Pauses the marquee animation. */
    void pause();

    /** Stops (and clears) the marquee animation */
    void stop();

    /** Sets refresh delay for the marquee animation */
    void setRefreshDelay(int refreshDelay);

    /**
     * Sets the text to display in the marquee. Forcibly stops the animation if
     * it is currently playing. TODO: In this case, should it auto-start
     * afterwards or does it have to wait for another start_marquee command?
     *
     * @param text  The text to show in the animation
     */
    void setText(std::string text);

  private:
    WINDOW *outWindow;

    int refreshDelay, screenWidth;

    bool running;

    std::vector<std::string> asciiText;

    std::mutex mymutex;

    std::condition_variable mycond;

    bool flag;

    std::thread animThread;

    DisplayManager &dm;
};

#endif
