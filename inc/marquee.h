#ifndef MARQUEE_H
#define MARQUEE_H

#include <curses.h>
#include <string>
#include <vector>
#include <atomic>

#include "../inc/process.h"

const int DEFAULT_SCREEN_WIDTH = 80;
class Marquee
{
  public:
    Marquee ( WINDOW *outWindow,
              int screenWidth = DEFAULT_SCREEN_WIDTH);

    ~Marquee();

    void start();   // Launches the thread
    void stop();    // Kills the thread
    void pause();   // Pauses the thread
    void resume();  // Resume if paused
    void setText(const std::string& newText);
    void setRefreshDelay(int newRefreshDelay);
    bool isRunning() const;

  private:
    void run();
    WINDOW* outWindow;
    std::string text;
    int screenWidth;
    std::atomic<int> refreshDelay;
    std::atomic<bool> paused;

    Process process;

};
#endif

/**
 * Begins a marquee animation given a specified string and refresh delay.
 *
 * @param outWindow     The target curses window to output the animation to
 * @param text          The string to show in the animation
 * @param refreshDelay  The delay between each animation frame in ms
 * @param screenWidth   The width of the output window/screen
 
void startMarquee(WINDOW *outWindow,
                  std::string &text,
                  int &refreshDelay,
                  int screenWidth);
*/