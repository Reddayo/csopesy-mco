#ifndef MARQUEE_H
#define MARQUEE_H

#include <curses.h>
#include <string>
#include <vector>
#include <atomic>

#include "../inc/process.h"

const int DEFAULT_SCREEN_WIDTH = 80;
const int DEFAULT_REFRESH_DELAY = 100;
const int DEFAULT_FONT_HEIGHT = 6;

class Marquee : public Process {
  public:
    Marquee ( WINDOW *outWindow,
              int screenWidth = DEFAULT_SCREEN_WIDTH) ;

    ~Marquee();

    void setText(const std::string& newText);
    void setRefreshDelay(int newRefreshDelay);

  private:
    void run() override;

    WINDOW* outWindow;
    int screenWidth;
    std::atomic<int> refreshDelay;
    std::vector<std::string> asciiArtRef;

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