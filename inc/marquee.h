#ifndef MARQUEE_H
#define MARQUEE_H

#include <condition_variable>
#include <curses.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class Marquee
{
  public:
    /**
     * Creates a new marquee instance.
     *
     * @param outWindow     The target curses window for output
     * @param refreshDelay  The delay between each animation frame in ms
     */
    Marquee(WINDOW *outWindow);

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
};

#endif
