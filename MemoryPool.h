//
// Created by stars on 2025/9/4.
//

#ifndef MEMORYPOOL_MEMORYPOOL_H
#define MEMORYPOOL_MEMORYPOOL_H
#include <cstddef>

namespace MemoryPool
{
    struct Slot  // 块
    {
        Slot* next;
    };
/**
 * @brief 内存池类，用于高效管理小块内存的分配和释放
 */
    class MemoryPool
    {
    private:
        size_t poolSize;        // 内存池大小
        size_t slotSize;        // 每个块的大小
        size_t slotAmount;      // 块的数量
        char* firstPtr;         // 第一个块的地址
        char* lastPtr;          // 最后一个块的地址
        Slot* freeSlot;         // 被释放的块的链表
        char* curPtr;           // 第一个未被使用的块的地址
        size_t usedAmount;      // 已经使用的块的数量
    public:
        MemoryPool* nextPool;   // 向下一个内存池的指针
        MemoryPool* prevPool;   // 向上一个内存池的指针

        MemoryPool(char* memoryBlock, size_t poolSize, size_t slotSize);

        ~MemoryPool();

        void* allocate();

        bool deallocate(void* ptr);
    };
} // MemoryPool

#endif //MEMORYPOOL_MEMORYPOOL_H