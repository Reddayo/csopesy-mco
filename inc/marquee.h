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

    /** Stops (and clears) the marquee animation */
    void stop();

    /** Sets refresh delay for the marquee animation */
    void setRefreshDelay(int refreshDelay);

    /**
     * Sets the text to display in the marquee.
     *
     * @param text  The text to show in the animation
     */
    void setText(std::string text);

  private:
    DisplayManager &dm;

    int refreshDelay;

    int screenWidth;

    bool animationRunning;

    bool animationInterrupted;

    std::vector<std::string> asciiText;

    std::mutex mutex;

    std::condition_variable wakeUp;

    std::thread animThread;
};

#endif
