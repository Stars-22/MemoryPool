//
// Created by LinJun on 2025/9/23.
//

#ifndef CENTRALCACHE_H
#define CENTRALCACHE_H

#include "CacheBase.h"
#include "MemoryPools.h"

namespace MemoryPool
{
    // 中央缓存类，管理不同大小的内存池
    class CentralCache final : public CacheBase<MemoryPools_Lock, CentralCache>
    {
    private:
        friend struct GlobalStorage;
        CentralCache() = default;
        ~CentralCache() override { cleanup(); }
        //@OPTIMIZE: 把此互斥锁删掉
        std::mutex mutex_;

        MemoryPool* allocatePool(size_t objSize) override;
        void deallocatePool(MemoryPool* pool) override;

    public:
        constexpr static size_t mul = EACH_POOL_SLOT_NUM;
        CentralCache(const CentralCache&) = delete;
        CentralCache& operator=(const CentralCache&) = delete;

        void* allocate(size_t objSize) override;
        void deallocate(void* ptr, size_t objSize) override;
    };
} // namespace MemoryPool

#endif // CENTRALCACHE_H
