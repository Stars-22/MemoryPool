/**
 * @file MemoryPool.h
 * @brief 内存池核心类定义
 */

#ifndef MEMORYPOOL_MEMORYPOOL_H
#define MEMORYPOOL_MEMORYPOOL_H
#include <cstddef>

namespace MemoryPool
{
    /**
     * @brief 内存池类，用于高效管理小块内存的分配和释放
     */
    class MemoryPool
    {
    private:
        /**
         * @brief 内存块结构体，用于构建空闲块链表
         */
        struct Slot
        {
            Slot* next; // 指向下一个空闲块
        };

        char* firstPtr;    // 内存池起始地址
        char* lastPtr;     // 最后一个可用块的地址
        Slot* freeSlot;    // 空闲块链表头
        char* curPtr;      // 当前未分配区域的起始地址
        size_t poolSize;   // 内存池总大小
        size_t slotSize;   // 单个块大小
        size_t slotAmount; // 总块数量
        size_t usedAmount; // 已使用块数量
    public:
        MemoryPool* nextPool; // 下一个内存池指针
        MemoryPool* prevPool; // 上一个内存池指针

        /**
         * @brief 构造函数
         * @param memoryBlock 内存块起始地址
         * @param poolSize 内存池大小
         * @param slotSize 单个块大小
         */
        MemoryPool(char* memoryBlock, size_t poolSize, size_t slotSize);

        ~MemoryPool() = default;

        /**
         * @brief 从内存池分配一个内存块
         * @return 分配的内存块指针，失败返回nullptr
         */
        void* allocate();

        /**
         * @brief 释放内存块回内存池
         * @param ptr 要释放的内存块指针
         * @return 如果内存池已空返回true，否则返回false
         */
        bool deallocate(void* ptr);

        [[nodiscard]] void* getFirstPtr() const;

        [[nodiscard]] size_t getPoolSize() const;

        size_t getSlotAmount() const { return slotAmount; }
        size_t getUsedAmount() const { return usedAmount; }

        size_t getSlotSize() const { return slotSize; }
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_MEMORYPOOL_H
