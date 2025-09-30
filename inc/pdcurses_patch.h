#ifndef PDCURSES_PATCH_H
#define PDCURSES_PATCH_H

/* This is included to support installation of PDCurses through MinGW on
 * Windows, as the endwin() function does not appear to be defined in curses.h
 * for whatever reason.
 */
#include <pdcurses.h>

#endif
