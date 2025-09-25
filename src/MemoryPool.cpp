/**
 * @file MemoryPool.cpp
 * @brief 内存池类实现
 */

#include "../include/MemoryPool.h"
#include <cassert>
#include <cstring>

namespace MemoryPool
{
    MemoryPool::MemoryPool(char* firstPtr, const size_t poolSize, const size_t slotSize) :
        firstPtr(firstPtr), poolSize(poolSize), slotSize(slotSize)
    {
        // 计算块的数量
        slotAmount = poolSize / slotSize;
        // 断言，如果块的数量小于1，则抛出异常
        assert(slotAmount > 0 && "Pool size too small for even one slot");
        // 断言，如果块的大小不是slot大小的倍数，则抛出异常
        assert(poolSize % slotSize == 0 && "Pool size must be multiple of slot size");

        lastPtr = firstPtr + poolSize - slotSize;
        freeSlot = nullptr;
        curPtr = firstPtr;
        nextPool = nullptr;
        prevPool = nullptr;
        usedAmount = 0;
    }

    void* MemoryPool::allocate()
    {
        if (usedAmount >= slotAmount)
            return nullptr;

        // 优先从空闲链表分配
        if (freeSlot != nullptr)
        {
            void* ptr = freeSlot;
            freeSlot = freeSlot->next;
            memset(ptr, 0, slotSize);
            usedAmount++;
            return ptr;
        }

        // 从未使用区域分配
        if (curPtr <= lastPtr)
        {
            void* ptr = curPtr;
            curPtr += slotSize;
            memset(ptr, 0, slotSize);
            usedAmount++;
            return ptr;
        }
        return nullptr;
    }

    bool MemoryPool::deallocate(void* ptr)
    {
        // 检查指针是否在分配的内存范围内
        assert(ptr >= firstPtr && ptr < lastPtr && "Pointer out of range");
        // 检查指针是否对齐到槽大小(slot size)
        assert((static_cast<char*>(ptr) - firstPtr) % slotSize == 0 && "Pointer not aligned to slot size");
        // 检查是否有已使用的槽位
        assert(usedAmount > 0 && "No slots used");

        usedAmount--;
        static_cast<Slot*>(ptr)->next = freeSlot;
        freeSlot = static_cast<Slot*>(ptr);
        return usedAmount == 0;
    }

    void* MemoryPool::getFirstPtr() const { return firstPtr; }

    size_t MemoryPool::getPoolSize() const { return poolSize; }
} // namespace MemoryPool
