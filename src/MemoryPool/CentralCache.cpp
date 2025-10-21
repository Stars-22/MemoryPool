//
// Created by stars on 2025/10/12.
//

#include "../../include/MemoryPool/CentralCache.h"
#include "../../include/MemoryPool/PageCache.h"

namespace MemoryPool
{
    MemoryPool* CentralCache::allocatePool(const size_t objSize)
    {
        size_t totalSize = objSize * EACH_POOL_SLOT_NUM;
        if (totalSize % EACH_PAGE_SIZE != 0)
        {
            totalSize = (totalSize + EACH_PAGE_SIZE) / EACH_PAGE_SIZE * EACH_PAGE_SIZE;
        }
        auto* slot = static_cast<char*>(PageCache::getCache()->allocate(totalSize));
        if (slot == nullptr)
            return nullptr;
        const auto pool = new MemoryPool(slot, totalSize, objSize);
        pools[objSize / (ALIGN * mul) - 1]->addPool(pool);
        return pool;
    }

    void CentralCache::deallocatePool(MemoryPool* pool)
    {
        size_t totalSize = pool->getPoolSize();
        PageCache::getCache()->deallocate(pool->getFirstPtr(), totalSize);
        delete pool;
    }

    void* CentralCache::allocate(size_t objSize)
    {
        std::lock_guard lock(mutex_);
        void* ptr = CacheBase::allocate(objSize);
        return ptr;
    }
    void CentralCache::deallocate(void* ptr, size_t objSize)
    {
        std::lock_guard lock(mutex_);
        CacheBase::deallocate(ptr, objSize);
    }
} // namespace MemoryPool
