//
// Created by stars on 2025/10/12.
//

#include "../../include/MemoryPool/ThreadCache.h"
#include <array>
#include "../../include/MemoryPool/CentralCache.h"
#include "../../include/MemoryPool/PageCache.h"
#include "../../include/config.h"

namespace MemoryPool
{
    MemoryPool* ThreadCache::allocatePool(const size_t objSize)
    {
        auto* slot = static_cast<char*>(CentralCache::getCache()->allocate(objSize * EACH_POOL_SLOT_NUM));
        if (slot == nullptr)
            return nullptr;
        const auto pool = new MemoryPool(slot, objSize * EACH_POOL_SLOT_NUM, objSize);
        pools[objSize / ALIGN - 1]->addPool(pool);
        return pool;
    }

    void ThreadCache::deallocatePool(MemoryPool* pool)
    {
        CentralCache::getCache()->deallocate(pool->getFirstPtr(), pool->getPoolSize());
        delete pool;
    }

    void* ThreadCache::allocate(const size_t objSize)
    {
        if (objSize > MAX_SLOT_SIZE)
        {
            return PageCache::getCache()->allocate(objSize);
        }
        return CacheBase::allocate(objSize);
    }

    void ThreadCache::deallocate(void* ptr, const size_t objSize)
    {
        if (objSize > MAX_SLOT_SIZE)
        {
            PageCache::getCache()->deallocate(ptr, objSize);
        }
        else CacheBase::deallocate(ptr, objSize);
    }
} // namespace MemoryPool
