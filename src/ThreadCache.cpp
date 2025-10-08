/**
 * @file ThreadCache.cpp
 * @brief 线程本地内存缓存实现
 */

#include "../include/ThreadCache.h"
#include "../include/PageCache.h"
#include <cassert>
#include <cstdlib>


namespace MemoryPool
{
    thread_local ThreadCache* ThreadCache::cache = nullptr;

    ThreadCache::ThreadCache() : pools{}
    {
        for (auto& poolGroup : pools)
        {
            poolGroup = new MemoryPools();
        }
    }

    ThreadCache::~ThreadCache()
    {
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

    void* ThreadCache::allocate(const size_t objSize)
    {
        // 如果大于最大内存池大小，则直接从PageCache中分配
        if (objSize > MAX_SLOT_SIZE)
        {
            return allocateFromPageCache(objSize);
        }

        size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        size_t slotSize = (slotNum + 1) * ALIGN;
        MemoryPool* pool = getCache()->pools[slotNum]->firstPool;
        // 用于存储分配对象的指针
        void* obj = nullptr;

        // 尝试从现有内存池分配
        if (pool != nullptr)
        {
            if ((obj = pool->allocate()) != nullptr)
                return obj;
            while (pool->nextPool != nullptr)
            {
                pool = pool->nextPool;
                if ((obj = pool->allocate()) != nullptr)
                    return obj;
            }
        }

        // 分配新内存池
        pool = allocatePool(slotSize);
        assert(pool != nullptr && "Failed to allocate pool");
        if ((obj = pool->allocate()) != nullptr)
            return obj;
        return nullptr;
    }

    void ThreadCache::deallocate(void* ptr, const size_t objSize)
    {
        // 如果大于最大内存池大小，则直接释放到PageCache中
        if (objSize > MAX_SLOT_SIZE)
        {
            deallocateToPageCache(ptr, objSize);
        }
        assert(ptr != nullptr && "ptr is nullptr");

        size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        MemoryPools* pools = getCache()->pools[slotNum];
        // 查找所在的内存池
        auto it = pools->poolsMap.lower_bound(static_cast<char*>(ptr));
        assert(it != pools->poolsMap.end() && "pool is nullptr");

        MemoryPool* pool = it->second;
        if (pool->deallocate(ptr))
        {
            // 内存池已空，释放资源
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
            pools->poolsMap.erase(it);
            delete pool;
        }
    }

    MemoryPool* ThreadCache::allocatePool(const size_t objSize)
    {
        char* slot = static_cast<char*>(std::malloc(objSize * EACH_POOL_SLOT_NUM));
        // char* slot = CenterCache::getCache()->allocate(slotSize * EACH_POOL_SLOT_NUM);
        if (slot == nullptr)
            return nullptr;

        const auto pool = new MemoryPool(slot, objSize * EACH_POOL_SLOT_NUM, objSize);
        const size_t slotNum = objSize / ALIGN - 1;

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

    void ThreadCache::deallocatePool(void* ptr, const size_t objSize)
    {
        // CenterCache::getCache()->deallocate(ptr, objSize);
        std::free(ptr);
    }

    void* ThreadCache::allocateFromPageCache(const size_t objSize)
    {
        return PageCache::getCache()->allocate(objSize);
    }

    void ThreadCache::deallocateToPageCache(void* ptr, const size_t objSize)
    {
        PageCache::getCache()->deallocate(ptr, objSize);
    }
} // namespace MemoryPool
