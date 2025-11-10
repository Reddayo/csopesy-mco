#include "../inc/backing_store.h"
#include <iomanip>

BackingStore::BackingStore(const std::string& filename, uint32_t frameSize)
            : filename(filename), frameSize(frameSize)
{
    // If file exists or create new
    std::ofstream ofs(filename, std::ios::app);
    ofs.close();
};

// Page OUT: write frame from RAM to disk     

// data points to an address from RAM
// write it to backing store
void BackingStore::pageOut(int processID, int pageNumber, const uint8_t* data){

    std::lock_guard<std::mutex> lock(mutex);

    std::fstream file(filename, std::ios::in | std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Backing store file not found. why'd you delete it???");
    }

    // Should output something like:
    // "Process 810 Page 556 : 49 20 6C 6F 76 65 20 48 61 61 74 6F"

    std::pair<int,int> key = {processID, pageNumber};
    std::streampos offset;

    auto it = pageMap.find(key);
    if (it != pageMap.end()) {
        offset = it->second;   // if exist overwrite else append
    } else {
        file.seekp(0, std::ios::end);
        offset = file.tellp();
        pageMap[key] = offset;
    }

    // Move to the offset and write the page
    file.seekp(offset);

    std::ostringstream oss;

    oss << "Process " << processID << " Page " << pageNumber << " : ";
    for (uint32_t i = 0; i < frameSize; i++) {
        oss << std::hex << std::uppercase
            << std::setw(2) << std::setfill('0')
            << static_cast<int>(data[i]);
        if (i < frameSize - 1) oss << ' ';
    }
    oss << std::dec << "\n";

    file.write(oss.str().c_str(), oss.str().size());
    file.flush();
    file.close();

};

// Page IN: load frame from disk to RAM
bool BackingStore::pageIn(int processID, int pageNumber, uint8_t* dest){
    std::lock_guard<std::mutex> lock(mutex);

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Backing store file not found. why'd you delete it???");
    }
    std::pair<int,int> key = {processID, pageNumber};

    auto it = pageMap.find(key);
    if (it == pageMap.end()) {
        // 404 Page not found
        std::ostringstream oss;
        oss << "404 Page not found in backing store : Process"
            << processID << " Page " << pageNumber;
        throw std::runtime_error(oss.str());
    }

    file.seekg(it->second);

    std::string line;
    std::getline(file, line);
    size_t pos = line.find(" : ");
    if (pos == std::string::npos) return false;
    std::string hexData = line.substr(pos + 3);

    std::istringstream iss(hexData);
    for (uint32_t i = 0; i < frameSize; i++) {
        std::string byteStr;
        if (!(iss >> byteStr)) return false;

        dest[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
    }

    return true;
    
};

// Remove them from the map and file once a process ends, some sort of compaction but that would be EXPENSIVE
// Just delete them.
void BackingStore::deleteProcessPages(int processID){

   return;
};
