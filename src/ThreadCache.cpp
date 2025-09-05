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
        : pools{}
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
        // 如果大于最大内存池大小，则直接从PageCache中分配
        if (objSize > MAX_SLOT_SIZE)
        {
            return allocateFromPageCache(objSize);
        }
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
                pool = pool->nextPool;
                if ((obj = pool->allocate()) != nullptr) return obj;
            }
        }
        // 如果所有现有内存池都分配失败，则请求一个新的内存池
        pool = allocatePool(slotSize);
        assert(pool != nullptr && "Failed to allocate pool");
        // 尝试从新分配的内存池中分配空间
        if ((obj = pool->allocate()) != nullptr) return obj;
        // 如果所有尝试都失败，返回nullptr
        return nullptr;
    }

    void ThreadCache::deallocate(void* ptr, size_t objSize)
    {
        // 如果大于最大内存池大小，则直接释放到PageCache中
        if (objSize > MAX_SLOT_SIZE)
        {
            deallocateToPageCache(ptr, objSize);
        }
        assert(ptr != nullptr && "ptr is nullptr");
        // 计算槽大小，通过将对象大小向上取整到8的倍数并除以8
        size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        MemoryPools* pools = getCache()->pools[slotNum];
        // 查找所在的内存池
        auto it = pools->poolsMap.lower_bound(static_cast<char*>(ptr));
        assert(it != pools->poolsMap.end() && "pool is nullptr");
        // 将对象指针返回给内存池
        MemoryPool* pool = it->second;
        if (pool->deallocate(ptr))
        {
            // 如果内存池已已清空，则释放此内存池
            deallocatePool(pool->getFirstPtr(), pool->getPoolSize());
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
            pools->poolsMap.erase(it);
            delete pool;
        }
    }

    MemoryPool* ThreadCache::allocatePool(size_t slotSize)
    {
        char* slot = static_cast<char*>(std::malloc(slotSize * EACH_POOL_SLOT_NUM));
        //char* slot = CenterCache::getCache()->allocate(slotSize * EACHPOOL_SLOT_NUM);
        if (slot == nullptr) return nullptr;
        const auto pool = new MemoryPool(slot, slotSize * EACH_POOL_SLOT_NUM, slotSize);
        const size_t slotNum = slotSize / ALIGN - 1;
        if (auto temp = getCache()->pools[slotNum]->firstPool; temp == nullptr)
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
        getCache()->pools[slotNum]->poolsMap[slot] = pool;
        return pool;
    }

    void ThreadCache::deallocatePool(void* ptr, size_t objSize)
    {
        //CenterCache::getCache()->deallocate(ptr, objSize);
        std::free(ptr);
    }

    void* ThreadCache::allocateFromPageCache(size_t objSize)
    {
        //return PageCache::getCache()->allocate(objSize);
    }

    void ThreadCache::deallocateToPageCache(void* ptr, size_t objSize)
    {
        //PageCache::getCache()->deallocate(ptr, objSize);
    }
} // MemoryPool
