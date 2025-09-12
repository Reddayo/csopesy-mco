#ifndef MARQUEE_H
#define MARQUEE_H

#include <curses.h>
#include <string>
#include <vector>

/**
 * Begins a marquee animation given a specified string and refresh delay.
 *
 * @param outWindow     The target curses window to output the animation to
 * @param text          The string to show in the animation
 * @param refreshDelay  The delay between each animation frame in ms
 * @param screenWidth   The width of the output window/screen
 */
void startMarquee(WINDOW *outWindow,
                  std::string &text,
                  int refreshDelay,
                  int screenWidth);

#endif