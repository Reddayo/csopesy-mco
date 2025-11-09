#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>

#include "../inc/config.h"
#include "../inc/process.h"

struct Frame {
    bool isOccupied;   // true if the frame is allocated
    bool isDirty;      // true if the page was modified and needs writing to disk
    bool isReferenced; // used by page replacement algorithms

    int ownerPID;      // Process ID that owns this frame
    int virtualPage;   // The virtual page number mapped to this physical frame
};

class MemoryManager {
    public:
        /* Use the config to initialize Backing Store, Main Memory, Frame Table */
        MemoryManager(Config& config);
        
        // Load a process into memory
        void loadProcess(Process& process);

        // Handle a page fault when a process accesses a non-resident page
        void handlePageFault(Process& process, uint32_t logicalAddress);

    private:
        uint32_t memory_size;        // total physical memory size in bytes
        uint32_t page_size;         // size of each page/frame in bytes
        uint32_t numFrames;        // total number of frames in memory

        std::unique_ptr<uint8_t[]> memory;  // actual physical memory array
        std::vector<Frame> frameTable;      // metadata for each physical frame
        BackingStore backingStore;         // secondary storage

        std::mutex mutex;
        // not sure, will figure out
        int findFreeFrame();                // find an unused frame in memory

        void allocateFrame(Process& process, int pageNumber); // assign frame to process page
};
