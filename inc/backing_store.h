#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <mutex>

class BackingStore {
public:
    BackingStore(const std::string& filename, uint32_t frameSize);

    // Reason we need to this: One txt file named "csopesy-backing-store.txt"
    // Figure 9.20 Dinosaur book
    // Page OUT: write frame from RAM to disk
    void pageOut(int processId, int pageNumber, const uint8_t* data);

    // Page IN: load frame from disk to RAM
    bool pageIn(int processId, int pageNumber, uint8_t* dest);

    // Remove them from the map and file once a process ends
    void deleteProcessPages(int processId);
private:
    std::string filename;
    uint32_t frameSize;
    std::streampos writePointer = 0;
    std::mutex mutex; //if needed idk, just add it

                        // key                 //value      //hash_func    
    std::unordered_map<std::pair<int, int>, std::streampos, pair_hash> pageMap;

    struct pair_hash {
        std::size_t operator()(const std::pair<int, int>& p) const noexcept {
            return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
        }
    };

    std::streampos getOffset(int processId, int pageNumber);
};
