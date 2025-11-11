#include "../inc/memory_manager.h"

MemoryManager::MemoryManager( const std::string &filename, uint32_t memory_size, uint32_t frame_size)
    : memory_size(memory_size),
      frame_size(frame_size),
      numFrames(memory_size / frame_size),
      backingStore(filename, frame_size)
{
    this->ageCounter = 0;
    memory = std::make_unique<uint8_t[]>(memory_size);

    frameTable.resize(numFrames);
    for (int i = 0; i < numFrames; i++) {
        frameTable[i].isOccupied = false;
        frameTable[i].isDirty = false;
        frameTable[i].isReferenced = false;
        frameTable[i].ownerPID = -1;
        frameTable[i].virtualPage = -1;
        frameTable[i].age = ageCounter;
    }
}


int MemoryManager::findLRUFrame() {
    uint64_t minTime = ageCounter;
    int lruIndex = 0;
    for (int i = 0; i < frameTable.size(); i++) {
        if (!frameTable[i].isOccupied) 
            return i;

        if (frameTable[i].age < minTime) {
            minTime = frameTable[i].age;
            lruIndex = i;
        }
    }
    return lruIndex;
}

int MemoryManager::allocateFrame(int processId, int pageNumber) {
    int frameIndex = findLRUFrame();

    // If the frame is occupied, page it out first
    if (frameTable[frameIndex].isOccupied && frameTable[frameIndex].isDirty) {
        backingStore.pageOut(
            frameTable[frameIndex].ownerPID,
            frameTable[frameIndex].virtualPage,
            memory.get() + frameIndex * frame_size
        );
    }

    // Load requested page into the frame
    backingStore.pageIn(
        processId,
        pageNumber,
        memory.get() + frameIndex * frame_size
    );

    frameTable[frameIndex].isOccupied = true;
    frameTable[frameIndex].isDirty = false;
    frameTable[frameIndex].isReferenced = true;
    frameTable[frameIndex].ownerPID = processId;
    frameTable[frameIndex].virtualPage = pageNumber;
    frameTable[frameIndex].age = ageCounter++;

    return frameIndex;
}


int MemoryManager::findFrame(int processId, uint32_t pageNumber){

    for (int i = 0; i < frameTable.size(); i++) {
        if (frameTable[i].isOccupied &&
            frameTable[i].ownerPID == processId &&
            frameTable[i].virtualPage == pageNumber)
        {
            return i;
        }
    }

    return -1;
    
}

uint16_t MemoryManager::read(int processId, uint32_t logicalAddress, int n) {
    uint16_t value = 0;

    for (int i = 0; i < n; i++) {
        uint32_t addr = logicalAddress + i;
        uint32_t pageNumber = addr / frame_size;
        uint32_t offset = addr % frame_size;

        int frameIndex = findFrame(processId, pageNumber);

         // page not in ram, page fault
        if (frameIndex == -1)
            frameIndex = allocateFrame(processId, pageNumber);

        frameTable[frameIndex].age = ageCounter++;
        frameTable[frameIndex].isReferenced = true;

        uint8_t byte = memory[frameIndex * frame_size + offset];

        value |= (byte << (8 * i));
    }

    return value;
}

void MemoryManager::write(int processId,  uint32_t logicalAddress, uint16_t value, int n) {
    for (int i = 0; i < n; i++) {
        uint8_t byte = (value >> (8 * i)) & 0xFF;

        uint32_t addr = logicalAddress + i;
        uint32_t pageNumber = addr / frame_size;
        uint32_t offset = addr % frame_size;

        int frameIndex = findFrame(processId, pageNumber);

        // page not in ram, page fault
        if (frameIndex == -1)
            frameIndex = allocateFrame(processId, pageNumber);

        memory[frameIndex * frame_size + offset] = byte;

        frameTable[frameIndex].isDirty = true;
        frameTable[frameIndex].age = ageCounter++;
        frameTable[frameIndex].isReferenced = true;
    }
}