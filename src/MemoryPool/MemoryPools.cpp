//
// Created by stars on 2025/10/12.
//

#include "../../include/MemoryPool/MemoryPools.h"
#include <cassert>
#include "../../include/MemoryPool/PageCache.h"
#include "../../include/config.h"

namespace MemoryPool
{
    MemoryPools::MemoryPools(const size_t size) : poolSize(size) {}

    MemoryPools::~MemoryPools()
    {
        poolsMap.clear();
        firstPool = nullptr;
    }

    void* MemoryPools::allocate() const
    {
        if (MemoryPool* pool = firstPool; pool != nullptr)
        {
            void* obj = nullptr;
            if ((obj = pool->allocate()) != nullptr)
                return obj;
            while (pool->nextPool != nullptr)
            {
                pool = pool->nextPool;
                if ((obj = pool->allocate()) != nullptr)
                    return obj;
            }
        }
        return nullptr;
    }

    MemoryPool* MemoryPools::deallocate(void* ptr)
    {
        auto it = poolsMap.upper_bound(static_cast<char*>(ptr));
        assert(it != poolsMap.begin() && "pool is nullptr");
        --it;
        if (MemoryPool* pool = it->second; pool->deallocate(ptr))
        {
            if (pool != firstPool)
            {
                pool->prevPool->nextPool = pool->nextPool;
                if (pool->nextPool != nullptr)
                    pool->nextPool->prevPool = pool->prevPool;
            }
            else
            {
                firstPool = pool->nextPool;
            }
            poolsMap.erase(it);
            return pool;
        }
        return nullptr;
    }

    void MemoryPools::addPool(MemoryPool* pool)
    {
        if (auto temp = firstPool; temp == nullptr)
        {
            firstPool = pool;
        }
        else
        {
            while (temp->nextPool != nullptr)
                temp = temp->nextPool;
            temp->nextPool = pool;
            pool->prevPool = temp;
        }
        poolsMap[static_cast<char*>(pool->getFirstPtr())] = pool;
    }


    MemoryPools_Lock::MemoryPools_Lock(const size_t size) : MemoryPools(size) {}

    void* MemoryPools_Lock::allocate()
    {
        std::lock_guard lock(mutex_);
        void* obj = MemoryPools::allocate();
        if (obj != nullptr)
            return obj;
        // 分配新内存池
        MemoryPool* pool = allocatePool();
        assert(pool != nullptr && "Failed to allocate pool");
        if ((obj = pool->allocate()) != nullptr)
            return obj;
        return nullptr;
    }

    void MemoryPools_Lock::deallocate(void* ptr)
    {
        assert(ptr != nullptr && "ptr is nullptr");
        std::lock_guard lock(mutex_);
        if (const MemoryPool* pool = MemoryPools::deallocate(ptr))
            deallocatePool(pool);
    }

    void MemoryPools_Lock::addPool(MemoryPool* pool) { MemoryPools::addPool(pool); }

    MemoryPool* MemoryPools_Lock::allocatePool()
    {
        size_t totalSize = poolSize * EACH_POOL_SLOT_NUM;
        if (totalSize % EACH_PAGE_SIZE != 0)
        {
            totalSize = (totalSize + EACH_PAGE_SIZE) / EACH_PAGE_SIZE * EACH_PAGE_SIZE;
        }
        auto* slot = static_cast<char*>(PageCache::getCache()->allocate(totalSize));
        if (slot == nullptr)
            return nullptr;
        const auto pool = new MemoryPool(slot, totalSize, poolSize);
        addPool(pool);
        return pool;
    }

    void MemoryPools_Lock::deallocatePool(const MemoryPool* pool)
    {
        const size_t totalSize = pool->getPoolSize();
        PageCache::getCache()->deallocate(pool->getFirstPtr(), totalSize);
        delete pool;
    }
} // namespace MemoryPool
