//
// Created by LinJun on 2025/9/23.
//

#ifndef CENTRALCACHE_H
#define CENTRALCACHE_H

#include <CacheBase.h>
#include <MemoryPools.h>
#include <array>

namespace MemoryPool
{
    // 中央缓存类，管理不同大小的内存池
    class CentralCache final : public CacheBase<MemoryPools_Lock, CentralCache>
    {
    private:
        friend struct GlobalStorage;
        CentralCache() = default;
        MemoryPool* allocatePool(const size_t objSize) override
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

        void deallocatePool(MemoryPool* pool) override
        {
            PageCache::getCache()->deallocate(pool->getFirstPtr(), pool->getPoolSize());
            delete pool;
        }

    public:
        constexpr static size_t mul = EACH_POOL_SLOT_NUM;
        CentralCache(const CentralCache&) = delete;
        CentralCache& operator=(const CentralCache&) = delete;
        ~CentralCache() override { cleanup(); }

        void* allocate(const size_t objSize) override { return CacheBase::allocate(objSize); }
        void deallocate(void* ptr, const size_t objSize) override { CacheBase::deallocate(ptr, objSize); }
    };
} // namespace MemoryPool

#endif // CENTRALCACHE_H
