//
// Created by stars on 2025/10/12.
//

#include "../../include/MemoryPool/ThreadCache.h"
#include <cassert>
#include "../../include/MemoryPool/CentralCache.h"
#include "../../include/MemoryPool/PageCache.h"

namespace MemoryPool
{
    ThreadCache::ThreadCache() : pools{}
    {
        for (size_t i = 0; i < pools.size(); ++i)
            pools[i] = new MemoryPools((i + 1) * ALIGN);
    }
    ThreadCache::~ThreadCache()
    {
        for (const auto& poolGroup : pools)
        {
            for (const auto& [key, pool] : poolGroup->poolsMap)
            {
                //@TODO: 此处是测试ThreadCache代码
                deallocatePool(pool);
            }
            delete poolGroup;
        }
    }

    ThreadCache* ThreadCache::getCache()
    {
        thread_local ThreadCache cache;
        return &cache;
    }

    MemoryPool* ThreadCache::allocatePool(const size_t objSize) const
    {
        auto* slot = static_cast<char*>(CentralCache::getCache()->allocate(objSize * EACH_POOL_SLOT_NUM_Thread));
        if (slot == nullptr)
            return nullptr;
        const auto pool = new MemoryPool(slot, objSize * EACH_POOL_SLOT_NUM_Thread, objSize);
        pools[objSize / ALIGN - 1]->addPool(pool);
        return pool;
    }

    void ThreadCache::deallocatePool(const MemoryPool* pool)
    {
        CentralCache::getCache()->deallocate(pool->getFirstPtr(), pool->getPoolSize());
        delete pool;
    }

    void* ThreadCache::allocate(size_t objSize) const
    {
        if (objSize > MAX_SLOT_SIZE)
        {
            if (objSize % EACH_PAGE_SIZE != 0)
            {
                objSize = (objSize + EACH_PAGE_SIZE) / EACH_PAGE_SIZE * EACH_PAGE_SIZE;
            }
            return PageCache::getCache()->allocate(objSize);
        }
        const size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        const size_t slotSize = (slotNum + 1) * ALIGN;
        void* obj = pools[slotNum]->allocate();
        if (obj != nullptr)
            return obj;
        // 分配新内存池
        MemoryPool* pool = allocatePool(slotSize);
        assert(pool != nullptr && "Failed to allocate pool");
        if ((obj = pool->allocate()) != nullptr)
            return obj;
        return nullptr;
    }

    void ThreadCache::deallocate(void* ptr, const size_t objSize) const
    {
        assert(ptr != nullptr && "ptr is nullptr");
        if (objSize > MAX_SLOT_SIZE)
        {
            PageCache::getCache()->deallocate(ptr, objSize);
            return;
        }
        const size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
        if (const MemoryPool* pool = pools[slotNum]->deallocate(ptr))
            deallocatePool(pool);
    }
} // namespace MemoryPool
