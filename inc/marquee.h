#ifndef MARQUEE_H
#define MARQUEE_H

#include <curses.h>
#include <string>
#include <vector>
#include <atomic>

#include "../inc/process.h"

const int DEFAULT_SCREEN_WIDTH = 80;
const int DEFAULT_REFRESH_DELAY = 10;
const int DEFAULT_FONT_HEIGHT = 6;

class Marquee {
  public:
    Marquee ( WINDOW *outWindow,
              int screenWidth = DEFAULT_SCREEN_WIDTH,
              int delayMs =  DEFAULT_REFRESH_DELAY ) ;

    ~Marquee();

    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning();

    void setText(const std::string& newText);
    void setRefreshDelay(int newRefreshDelay);

  private:
    void drawNextFrame();

    WINDOW* outWindow;
    int screenWidth;
    Process process;
    std::vector<std::string> asciiArtRef;
    std::vector<std::string> display;
    int currentDisplayCol;
    size_t rowCount;
    size_t rowLen;
    

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