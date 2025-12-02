#include <iostream>
#include <cstdint>
#include <stdexcept>

bool isPowerOf2NInRange(uint32_t memory_size){
    if (memory_size < (1 << 3) || memory_size > (1 << 16)){
        return false;
    }

    if (memory_size & (memory_size - 1)){
        return false;
    }
    return true;
}

/* Assumes memory_size in [64, 65536] 
   NOTE: this overflows to 2^32
*/
uint32_t nextPowerOf2(uint32_t memory_size) {
    if (memory_size == 0) return 1;

    memory_size--;
    memory_size |= memory_size >> 1;
    memory_size |= memory_size >> 2;
    memory_size |= memory_size >> 4;
    memory_size |= memory_size >> 8;
    memory_size |= memory_size >> 16;
    memory_size++;

    return (memory_size > 65536) ? 65536 : memory_size;
}