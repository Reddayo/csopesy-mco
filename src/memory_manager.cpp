#include "../inc/memory_manager.h"
#include "../inc/backing_store.h"

MemoryManager::MemoryManager(Config& config)
: memory_size(config.getMaxOverAllMem()),
  frame_size(config.getMemPerFrame()),
  numFrames(config.getNumFrames()), //probably not needed
  backingStore(BackingStore("csopesy-backing-store.txt", frame_size))
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

int MemoryManager::allocateFrame(Process& process, int pageNumber) {
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
        process.getId(),
        pageNumber,
        memory.get() + frameIndex * frame_size
    );

    frameTable[frameIndex].isOccupied = true;
    frameTable[frameIndex].isDirty = false;
    frameTable[frameIndex].isReferenced = true;
    frameTable[frameIndex].ownerPID = process.getId();
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

// TODO: Ask sir if there's gonna be a read/write of odd address
uint8_t MemoryManager::read(Process &process, uint32_t logicalAddress){
    uint32_t pageNumber = logicalAddress / frame_size;
    uint32_t offset = logicalAddress % frame_size;

    int frameIndex = findFrame(process.getId(), pageNumber);

    if (frameIndex != -1) 
    {
        frameTable[frameIndex].age = ageCounter++;
        frameTable[frameIndex].isReferenced = true;
        return memory[frameIndex * frame_size + offset];
    }
    
    frameIndex = allocateFrame(process, pageNumber);

    return memory[frameIndex * frame_size + offset];
};

void MemoryManager::write(Process &process, uint32_t logicalAddress, uint8_t value){

    uint32_t pageNumber = logicalAddress / frame_size;
    uint32_t offset = logicalAddress % frame_size;

    // Find page in RAM
    int frameIndex = findFrame(process.getId(), pageNumber);

    if (frameIndex != -1) 
    {
        memory[frameIndex * frame_size + offset] = value;
        frameTable[frameIndex].isDirty = true;
        frameTable[frameIndex].age = ageCounter++;
        frameTable[frameIndex].isReferenced = true;
        return;
        
    }
    // Page not in ram, page fault
    frameIndex = allocateFrame(process, pageNumber);

    memory[frameIndex * frame_size + offset] = value;
    frameTable[frameIndex].isDirty = true;
    frameTable[frameIndex].age = ageCounter++;
    frameTable[frameIndex].isReferenced = true;

};