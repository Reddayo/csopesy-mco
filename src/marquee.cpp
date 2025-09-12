#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"

const int DEFAULT_FONT_HEIGHT = 6;
const int DEFAULT_REFRESH_DELAY = 100;
const int DEFAULT_SCREEN_WIDTH = 80;

void startMarquee (WINDOW *outWindow,
                   std::string &text,
                   int refreshDelay = DEFAULT_REFRESH_DELAY,
                   int screenWidth = DEFAULT_SCREEN_WIDTH)
{
    // Convert text string to ASCII art
    const std::vector<std::string> lines = convertToASCIIArt(text, screenWidth);

    if (lines.empty()) {
        // TODO: Do something when ASCII art is empty
        return;
    }

    const size_t rowCount = DEFAULT_FONT_HEIGHT;
    const size_t rowLen = lines[0].size();

    // I haven't processed what kind of black magic is happening from this point
    // onward. I just removed the std::cout stuff and replaced them with calls
    // to PDCurses to print to the window instead - lowest

    std::string seq; // Convert to 1D first, cause my brain would melt
    for (const auto &l : lines) {
        seq += l;
    }

    const size_t seqLen = seq.size();

    std::vector<std::string> display(rowCount, std::string(screenWidth, ' '));

    size_t pos = 0;
    while (true) {
        for (size_t row = 0; row < rowCount; row++) {
            display[row].erase(0, 1);
            size_t idx = (pos + row * rowLen) % seqLen;
            display[row].push_back(seq[idx]);
            mvwprintw(outWindow, row + 1, 1, display[row].c_str());
        }
        wrefresh(outWindow);
        pos = (pos + 1) % rowLen;
        std::this_thread::sleep_for(std::chrono::milliseconds(refreshDelay));
    }
}