//
// Created by stars on 2025/9/4.
//

#include "MemoryPool.h"

#include <iostream>
#include <ostream>

namespace MemoryPool
{
    MemoryPool::MemoryPool(char* firstPtr, size_t poolSize, size_t slotSize)
        :firstPtr(firstPtr), poolSize(poolSize), slotSize(slotSize){
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

    MemoryPool::~MemoryPool()
    {

    }

/**
 * 从内存池中分配一块内存空间
 * @return 指向分配的内存空间的指针，如果内存池已满则返回nullptr
 */
    void* MemoryPool::allocate(){
        if(usedAmount >= slotAmount) return nullptr;
        // 从释放链表中获取内存
        if(freeSlot != nullptr){
            void* ptr = freeSlot;
            freeSlot = freeSlot->next;
            memset(ptr, 0, slotSize);
            usedAmount++;
            return ptr;
        }
        // 从未被使用的地址获取内存
        if(curPtr < lastPtr){
            void* ptr = curPtr;
            curPtr += slotSize;
            memset(ptr, 0, slotSize);
            usedAmount++;
            return ptr;
        }
        return nullptr;
    }

/**
 * @brief 释放内存池中分配的内存块
 * @param ptr 指向要释放的内存块的指针
 * @note 该函数将内存块返回到内存池的空闲列表中，而不是操作系统
 * @return 是否可以释放此Pool
 */
    bool MemoryPool::deallocate(void* ptr){
        // 检查指针是否在分配的内存范围内
        assert(ptr >= firstPtr && ptr < lastPtr && "Pointer out of range");
        // 检查指针是否对齐到槽大小(slot size)
        assert(((char*)ptr - firstPtr) % slotSize == 0 && "Pointer not aligned to slot size");
        // 检查是否有已使用的槽位
        assert(usedAmount > 0 && "No slots used");
        usedAmount--;
        ((Slot*)ptr)->next = freeSlot;
        freeSlot = (Slot*)ptr;
        return usedAmount == 0;
    }



} // MemoryPool