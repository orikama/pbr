#include "memory.h"
//#include <cstdlib>    // std::aligned_alloc
#include <algorithm>    // std::max


PBR_NAMESPACE_BEGIN

inline
void* AllocAligned(std::size_t size)
{
#ifdef PBR_HAVE_ALIGNED_MALLOC
    return _aligned_malloc(size, PBR_L1_CACHE_LINE_SIZE);
#else
    // TODO: Add std::aligned_alloc
    #error "Only MSVC is supported for now"
#endif
}

inline
void FreeAligned(void* ptr)
{
    PBR_ASSERT(ptr != nullptr)
#ifdef PBR_HAVE_ALIGNED_MALLOC
    _aligned_free(ptr);
#else
    #error "Only MSVC is supported for now"
#endif
}


MemoryArena::MemoryArena(std::size_t blockSize)
    : m_blockSize(blockSize)
{}

MemoryArena::~MemoryArena()
{
    FreeAligned(m_currentBlock);
    for (auto& block : m_usedBlocks) FreeAligned(block.second);
    for (auto& block : m_availableBlocks) FreeAligned(block.second);
}

void* MemoryArena::Alloc(std::size_t nBytes)
{
    // DIFFERENCE: As I understand in the book they assume align=16, but msvc gives align=8. Does it affect anything ?
    constexpr i32 align = alignof(std::max_align_t);
    nBytes = (nBytes + align -1) & ~(align - 1);

    if (m_currentBlockPos + nBytes > m_currentBlockSize) {
        if (m_currentBlock != nullptr) {
            // IMPROVE: emplace_back ?
            m_usedBlocks.push_back(std::make_pair(m_currentBlockSize, m_currentBlock));
            m_currentBlock = nullptr;
        }

        // IMPROVE: std::find() or at least foreach ?
        // Check if there are any already allocated free blocks
        for (auto iter = m_availableBlocks.begin(); iter != m_availableBlocks.end(); ++iter)
            if (iter->first >= nBytes) {
                m_currentBlockSize = iter->first;
                m_currentBlock = iter->second;
                m_availableBlocks.erase(iter);
                break;
            }

        if (m_currentBlock == nullptr) {
            m_currentBlockSize = std::max(nBytes, m_blockSize);
            m_currentBlock = static_cast<ui8*>(AllocAligned(m_currentBlockSize));
        }

        m_currentBlockPos = 0;
    }

    void* ret = m_currentBlock + m_currentBlockPos;
    m_currentBlockPos += nBytes;

    return ret;
}

PBR_NAMESPACE_END
