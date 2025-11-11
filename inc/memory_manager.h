#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <optional>

#include "backing_store.h"

struct Page;

struct Frame {
    bool isOccupied;   // true if the frame is allocated
    bool isDirty;      // true if the page was modified and needs writing to disk
    bool isReferenced; // used by page replacement algorithms

    int ownerPID;      // Process ID that owns this frame
    int virtualPage;   // The virtual page number mapped to this physical frame
    uint64_t age;
};

class MemoryManager {
    public:
        /* Use the config to initialize Backing Store, Main Memory, Frame Table */
        MemoryManager(const std::string &filename, uint32_t memory_size, uint32_t frame_size);
        
        // Load a process into memory
        //void loadProcess(std::string processId, std::vector<Page> page_table);
        uint16_t read(int processId, uint32_t logicalAddress, int n);
        void write(int processId, uint32_t logicalAddress, uint16_t value, int n);

    private:
        uint32_t memory_size;        // total physical memory size in bytes
        uint32_t frame_size;         // size of each page/frame in bytes
        uint32_t numFrames;        // total number of frames in memory

        std::unique_ptr<uint8_t[]> memory;  // actual physical memory array
        std::vector<Frame> frameTable;      // metadata for each physical frame
        BackingStore backingStore;         // secondary storage

        std::mutex mutex;
        // not sure, will figure out
        uint64_t ageCounter = 0;
        int findFrame(int processId, uint32_t pageNumber);
        int findLRUFrame();                // find an unused frame in memory
        int allocateFrame(int processId, int pageNumber); // assign frame to process page
};

#endif