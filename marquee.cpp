#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <map>

const int DEFAULT_SCREEN_WIDTH = 50;
const int DEFAULT_REFRESH_DELAY = 100;
const int DEFAULT_FONT_HEIGHT = 6;
const int DEFAULT_SPACE_GAP = 4;
const std::string ANSI_ESCAPE = "\033[";
/* TODO: FILL THIS UP*/
const std::map<char, std::vector<std::string>> asciiArtMap = {
        {'A', {"     ___      ",
               "    /   \\     ",
               "   /  ^  \\    ",
               "  /  /_\\  \\   ",
               " /  _____  \\  ",
               "/__/     \\__\\ ",}},
        {'B', {".______   ",
               "|   _  \\  ",
               "|  |_)  | ",
               "|   _  <  ",
               "|  |_)  | ",
               "|______/  ",
                }},
        {'C', {"  ______ ",
               " /      |",
               "|  ,----'",
               "|  |     ",
               "|  `----.",
               " \\______|",}},
        {'P', {".______   ",
               "|   _  \\  ",
               "|  |_)  | ",
               "|   ___/  ",
               "|  |      ",
               "| _|    "}},


    };

std::vector<std::string> convertToAsciiArt(const std::string& text){
    if (text.empty()) return {};

    std::vector<std::string> displayText(DEFAULT_FONT_HEIGHT, "");

    for (char c : text) {
        auto it = asciiArtMap.find(c); // find returns an iterator that points to the element based on the key
        //.end() is the terminating "null byte" of maps

        if (it != asciiArtMap.end()) { // IF FOUND, add it to displayText,
            const auto& letter = it->second;
            for (size_t row = 0; row < DEFAULT_FONT_HEIGHT; row++) {
                displayText[row] += letter[row];
            }
        } else {    // IF NOT FOUND, add a space
            for (size_t row = 0; row < DEFAULT_FONT_HEIGHT; row++) {
                displayText[row] += std::string(DEFAULT_SPACE_GAP, ' ');
            }
        }
    }

    for (size_t row = 0; row < DEFAULT_FONT_HEIGHT; row++){
        displayText[row] += std::string(DEFAULT_SCREEN_WIDTH, ' ');
    }

    return displayText;
}


void startMarquee(const std::vector<std::string>& lines, int refreshDelay = DEFAULT_REFRESH_DELAY) {
    if (lines.empty()){
        std::cout << "ASCII Text art is empty\n";
        return;
    }
    
    const size_t rowCount = DEFAULT_FONT_HEIGHT;
    const size_t rowLen = lines[0].size();

 
    std::string seq; // Convert to 1D first, cause my brain would melt
    for (const auto& l : lines) {
        seq += l;
    }
    const size_t seqLen = seq.size();

    std::vector<std::string> display(rowCount, std::string(DEFAULT_SCREEN_WIDTH, ' '));

    size_t pos = 0;
    while (true) {
        for (size_t row = 0; row < rowCount; row++) {
            display[row].erase(0, 1);
            size_t idx = (pos + row * rowLen) % seqLen;
            display[row].push_back(seq[idx]);
            std::cout << display[row] << "\n";
        }

        std::cout << "\033[" << rowCount << "A" << std::flush;

        pos = (pos + 1) % rowLen;
        std::this_thread::sleep_for(std::chrono::milliseconds(refreshDelay));
    }
}

int main() {
    std::string text;

    /* TODO: commands, multithreading, and OS details*/

    std::cout << "Enter text to marquee: ";
    std::getline(std::cin, text);
    
    startMarquee(convertToAsciiArt(text));

    return 0;
}



/* 
void startMarquee(const std::string& text, int refreshDelay = DEFAULT_REFRESH_DELAY) {

    std::string display(DEFAULT_SCREEN_WIDTH, ' ');

    std::string seq = text + std::string(DEFAULT_SCREEN_WIDTH, ' ');

    size_t len = seq.length();

    size_t pos = 0;

    while (true) {
        display.erase(0, 1);         // remove first index
        display.push_back(seq[pos]); //append a new char base on pos

        std::cout << "\r" << display << std::flush;

        pos = (pos + 1) % len;
        std::this_thread::sleep_for(std::chrono::milliseconds(refreshDelay));
    }
}
*/