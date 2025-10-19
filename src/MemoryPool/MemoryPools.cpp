//
// Created by stars on 2025/10/12.
//

#include "../../include/MemoryPool/MemoryPools.h"
#include <cassert>

namespace MemoryPool
{
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
        bool a = poolsMap.empty();
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
            {
                temp = temp->nextPool;
            }
            temp->nextPool = pool;
            pool->prevPool = temp;
        }
        poolsMap[static_cast<char*>(pool->getFirstPtr())] = pool;
    }


    void* MemoryPools_Lock::allocate()
    {
        std::lock_guard lock(mutex_);
        return MemoryPools::allocate();
    }

    MemoryPool* MemoryPools_Lock::deallocate(void* ptr)
    {
        std::lock_guard lock(mutex_);
        return MemoryPools::deallocate(ptr);
    }

    void MemoryPools_Lock::addPool(MemoryPool* pool)
    {
        std::lock_guard lock(mutex_);
        MemoryPools::addPool(pool);
    }
} // namespace MemoryPool
