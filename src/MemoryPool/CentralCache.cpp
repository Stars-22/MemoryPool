//
// Created by stars on 2025/10/12.
//

#include "../../include/MemoryPool/CentralCache.h"
#include "../../include/MemoryPool/PageCache.h"

namespace MemoryPool
{
    CentralCache::CentralCache() : pools{}
    {
        for (size_t i = 0; i < pools.size(); ++i)
            pools[i] = new MemoryPools_Lock((i + 1) * EACH_POOL_SLOT_NUM_Thread * ALIGN);
    }
    CentralCache::~CentralCache()
    {
        for (auto& pool : pools)
            delete pool;
    }

    CentralCache* CentralCache::getCache()
    {
        static CentralCache cache;
        return &cache;
    }

    void* CentralCache::allocate(const size_t objSize) const
    {
        return pools[(objSize + ALIGN * EACH_POOL_SLOT_NUM_Thread - 1) / (ALIGN * EACH_POOL_SLOT_NUM_Thread) - 1]->allocate();
    }
    void CentralCache::deallocate(void* ptr, const size_t objSize) const
    {
        pools[(objSize + ALIGN * EACH_POOL_SLOT_NUM_Thread - 1) / (ALIGN * EACH_POOL_SLOT_NUM_Thread) - 1]->deallocate(ptr);
    }
} // namespace MemoryPool
