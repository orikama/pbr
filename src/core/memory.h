#pragma once

#include "core.hpp"
#include <list>     // std::list
#include <utility>  // std::pair

// FINDOUT: Is MemoryArena actually better than just 'new' ?

PBR_NAMESPACE_BEGIN

//void* AllocAligned(std::size_t size);
//void  FreeAligned(void* ptr);

// FINDOUT: Check if alignas() impact anything.
class alignas(PBR_L1_CACHE_LINE_SIZE) MemoryArena
{
public:
    MemoryArena(std::size_t blockSize);
    ~MemoryArena();

    MemoryArena(const MemoryArena&) = delete;
    MemoryArena& operator=(const MemoryArena&) = delete;

    template<typename T>
    T* Alloc(size_t size, bool runConstructor);

    size_t TotalAllocated() const;

private:
    void* Alloc(std::size_t bytes);


    const std::size_t m_blockSize;
    std::size_t m_currentBlockPos = 0;
    std::size_t m_currentBlockSize = 0;
    ui8* m_currentBlock = nullptr;
    // IMPROVE: I'm sure that std::forward_list would be enough.
    std::list<std::pair<std::size_t, ui8*>> m_usedBlocks;
    std::list<std::pair<std::size_t, ui8*>> m_availableBlocks;
};


template<typename T>
T* MemoryArena::Alloc(size_t size, bool runConstructor)
{
    T* ret = static_cast<T*>(Alloc(size * sizeof(T)));
    if (runConstructor)
        for (size_t i = 0; i < size; ++i)
            new (&ret[i]) T();

    return ret;
}

PBR_NAMESPACE_END
