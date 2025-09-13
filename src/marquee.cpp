#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"
#include "../inc/marquee.h"

const int DEFAULT_FONT_HEIGHT = 6;
const int DEFAULT_REFRESH_DELAY = 100;

Marquee::Marquee (  WINDOW *outWindow,
                    int screenWidth)
                    :   outWindow(outWindow),
                        screenWidth(screenWidth),
                        paused(false),
                        process([this]() { this->run(); }) {}
Marquee::~Marquee() {
    stop();
}

void Marquee::start() {
    paused = false;
    process.start();
}
void Marquee::stop() {
    process.stop();
}
void Marquee::pause() {
    paused = true;
}
void Marquee::resume() {
    paused = false; 
}

bool Marquee::isRunning() const{
    return process.isRunning();
}
void Marquee::setText(const std::string& newText) {

    /* Handle error detection here, instead of at the main thread*/
    text = newText;
}

void Marquee::setRefreshDelay(int newRefreshDelay) {
    refreshDelay.store(newRefreshDelay);

}

void Marquee::run() {
    const size_t rowCount = DEFAULT_FONT_HEIGHT;
    size_t col = 0;
    // Convert text string to ASCII art
    const std::vector<std::string> asciiArtRef = convertToASCIIArt(text, screenWidth);

    // 'display' is what the screen looks like,  
    // where each element of the vector represents a row,  
    // and each character in the string represents a column,  
    // initially filled with spaces to act as a blank canvas for drawing.
    // added explanation, hope this helps - red
    std::vector<std::string> display(rowCount, std::string(screenWidth, ' '));

    
    while (process.isRunning()) {
        if (paused) {
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