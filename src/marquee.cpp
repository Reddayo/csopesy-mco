#include <curses.h>
#include <string>
#include <thread>
#include <vector>

#include "../inc/ascii_map.h"
#include "../inc/marquee.h"


    // 'display' is what the screen looks like,  
    // where each element of the vector represents a row,  
    // and each character in the string represents a column,  
    // initially filled with spaces to act as a blank canvas for drawing.
    // added explanation, hope this helps - red

Marquee::Marquee ( WINDOW *outWindow,
                   int screenWidth, 
                   int delayMs)
                   : outWindow(outWindow), 
                     asciiArtRef({""}),
                     screenWidth(screenWidth), 
                     currentDisplayCol(0),
                     process([this]{ drawNextFrame(); }, delayMs), 
                     display(DEFAULT_FONT_HEIGHT, std::string(screenWidth, ' ')) {}

Marquee::~Marquee() {
    stop();
}


void Marquee::start() { 
    process.start(); 
}
void Marquee::stop() { 
    process.stop();
}

void Marquee::pause() { 
    process.pause();
}

void Marquee::resume() { 
    process.resume();
}

bool Marquee::isRunning(){
    return process.isRunning();
}

void Marquee::setText(const std::string& newText) {

    /* Handle error detection here, instead of at the main thread*/
    // Convert text string to ASCII art
    asciiArtRef = convertToASCIIArt(newText, screenWidth);
    rowLen = asciiArtRef[0].size();
    rowCount = asciiArtRef.size();
    currentDisplayCol = rowLen - screenWidth;
}

void Marquee::setRefreshDelay(int newRefreshDelay) {
    process.setDelayMs(newRefreshDelay);
}

void Marquee::drawNextFrame() {
    
    if (asciiArtRef.empty()) {
        // TODO: Do something when ASCII art is empty
        return;
    }  

    // I haven't processed what kind of black magic is happening from this point
    // onward. I just removed the std::cout stuff and replaced them with calls
    // to PDCurses to print to the window instead - lowest

    for (size_t row = 0; row < rowCount; row++) {
        for (size_t col = 0; col < screenWidth; col++) {
            size_t artCol = (currentDisplayCol + col) % rowLen;
            mvwaddch(outWindow, row + 1, col + 1, asciiArtRef[row][artCol]);
        }
    }
    wrefresh(outWindow);
    // To wrap once it reaches the end
    currentDisplayCol = (currentDisplayCol + 1) % rowLen;
        
}