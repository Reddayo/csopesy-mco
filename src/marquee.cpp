#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"
#include "../inc/marquee.h"


Marquee::Marquee (  WINDOW *outWindow,
                    int screenWidth)
                    :   Process(nullptr),
                        outWindow(outWindow),
                        screenWidth(screenWidth),
                        asciiArtRef({""}),
                        refreshDelay(DEFAULT_REFRESH_DELAY) {}
Marquee::~Marquee() {
    stop();
}

void Marquee::setText(const std::string& newText) {

    /* Handle error detection here, instead of at the main thread*/
    asciiArtRef = convertToASCIIArt(newText, screenWidth);

}

void Marquee::setRefreshDelay(int newRefreshDelay) {
    refreshDelay.store(newRefreshDelay);
}

void Marquee::run() {
    const size_t rowCount = DEFAULT_FONT_HEIGHT;
    size_t col = 0;
    // Convert text string to ASCII art
    

    // 'display' is what the screen looks like,  
    // where each element of the vector represents a row,  
    // and each character in the string represents a column,  
    // initially filled with spaces to act as a blank canvas for drawing.
    // added explanation, hope this helps - red
    std::vector<std::string> display(rowCount, std::string(screenWidth, ' '));

    
    while (isRunning()) {
        if (isPaused()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        if (asciiArtRef.empty()) {
            // TODO: Do something when ASCII art is empty
            return;
        }

        const size_t rowLen = asciiArtRef[0].size();

        // I haven't processed what kind of black magic is happening from this point
        // onward. I just removed the std::cout stuff and replaced them with calls
        // to PDCurses to print to the window instead - lowest

        for (size_t row = 0; row < rowCount; row++) {
            // Remove the leftmost character from this row
            display[row].erase(0, 1);
            // Add the next character from the ASCII art reference
            display[row].push_back(asciiArtRef[row][col]);
            mvwprintw(outWindow, row + 1, 1, display[row].c_str());
        }
        wrefresh(outWindow);
        // To wrap once it reaches the end
        col = (col + 1) % rowLen;
        std::this_thread::sleep_for(std::chrono::milliseconds(refreshDelay.load()));
        
    }
}