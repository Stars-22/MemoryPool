//
// Created by stars on 2025/9/4.
//

#include <cstdlib>
#include <cassert>
#include "../include/ThreadCache.h"

namespace MemoryPool
{
    thread_local ThreadCache* ThreadCache::cache = nullptr;

    ThreadCache::ThreadCache()
    {
        for (auto& poolGroup : pools)
        {
            poolGroup = new MemoryPools();
        }
    }

    ThreadCache::~ThreadCache()
    {
        // 清理所有内存池
        for (const auto& poolGroup : pools)
        {
            delete poolGroup;
        }
    }

    ThreadCache* ThreadCache::getCache()
    {
        if (cache == nullptr)
        {
            cache = new ThreadCache();
        }
        return cache;
    }

    void* ThreadCache::allocate(size_t objSize)
    {
        // 计算需要的槽大小，通过将类型T的大小向上取整到8的倍数并除以8
        size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        size_t slotSize = (slotNum + 1) * ALIGN;
        // 从缓存中获取对应槽大小的内存池
        MemoryPool* pool = getCache()->pools[slotNum]->firstPool;
        // 用于存储分配对象的指针
        void* obj = nullptr;
        // 尝试从当前内存池分配空间
        if (pool != nullptr)
        {
            if ((obj = pool->allocate()) != nullptr) return obj;
            // 如果当前内存池分配失败，则尝试后续的内存池
            while (pool->nextPool != nullptr)
            {
                if ((obj = pool->allocate()) != nullptr) return obj;
                pool = pool->nextPool;
            }
        }
        // 如果所有现有内存池都分配失败，则请求一个新的内存池
        pool = allocatePool(slotSize);
        // 尝试从新分配的内存池中分配空间
        if ((obj = pool->allocate()) != nullptr) return obj;
        // 如果所有尝试都失败，返回nullptr
        return nullptr;
    }

    void ThreadCache::deallocate(void* ptr, size_t objSize)
    {
        assert(ptr != nullptr && "ptr is nullptr");
        // 计算槽大小，通过将对象大小向上取整到8的倍数并除以8
        size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        char* ptr_ = (char*)ptr;
        MemoryPools* pools = getCache()->pools[slotNum];
        // 查找所在的内存池
        auto it = pools->poolsmap.lower_bound(ptr_);
        assert(it != pools->poolsmap.end() && "pool is nullptr");
        // 将对象指针返回给内存池
        MemoryPool* pool = it->second;
        if (pool->deallocate(ptr))
        {
            // 如果内存池已已清空，则释放此内存池
            deallocatePool(pool);
            if (pool != pools->firstPool)
            {
                pool->prevPool->nextPool = pool->nextPool;
                pool->nextPool->prevPool = pool->prevPool;
            }
            else
            {
                pools->firstPool = pool->nextPool;
            }
            // 从映射中移除
            pools->poolsmap.erase(it);
        }
    }

    MemoryPool* ThreadCache::allocatePool(size_t slotSize)
    {
        size_t slotNum = slotSize / ALIGN - 1;
        char* slot = static_cast<char*>(std::malloc(slotSize * EACHPOOL_SLOT_NUM));
        if (slot == nullptr) return nullptr;
        MemoryPool* pool = new MemoryPool(slot, slotSize * EACHPOOL_SLOT_NUM, slotSize);
        MemoryPool* temp = getCache()->pools[slotNum]->firstPool;
        if (temp == nullptr)
        {
            getCache()->pools[slotNum]->firstPool = pool;
        }
        else
        {
            while (temp->nextPool != nullptr)
            {
                temp = temp->nextPool;
            }
            temp->nextPool = pool;
            pool->prevPool = temp;
        }
        getCache()->pools[slotNum]->poolsmap[slot] = pool;
        return pool;
    }

    void ThreadCache::deallocatePool(MemoryPool* pool)
    {
    }
} // MemoryPool
