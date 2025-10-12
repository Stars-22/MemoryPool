/**
 * @file ThreadCache.h
 * @brief 线程本地内存缓存管理器
 */

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H

#include "CacheBase.h"
#include "MemoryPools.h"

namespace MemoryPool
{
    // 线程缓存类，用于管理线程本地的内存池
    class ThreadCache final : public CacheBase<MemoryPools, ThreadCache, ThreadLocalStorage>
    {
    private:
        friend struct ThreadLocalStorage;
        ThreadCache() = default;

        MemoryPool* allocatePool(size_t objSize) override;
        void deallocatePool(MemoryPool* pool) override;

    public:
        constexpr static size_t mul = 1;
        ThreadCache(const ThreadCache&) = delete;
        ThreadCache& operator=(const ThreadCache&) = delete;
        ~ThreadCache() override { cleanup(); }

        void* allocate(size_t objSize) override;
        void deallocate(void* ptr, size_t objSize) override;
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_THREADCACHE_H
