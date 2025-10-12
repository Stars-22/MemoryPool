/**
 * @file ThreadCache.h
 * @brief 线程本地内存缓存管理器
 */

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H

#include <array>
#include "CacheBase.h"
#include "MemoryPools.h"
#include "PageCache.h"
#include "CentralCache.h"
#include "config.h"

namespace MemoryPool
{
    // 线程缓存类，用于管理线程本地的内存池
    class ThreadCache final : public CacheBase<MemoryPools, ThreadCache, ThreadLocalStorage>
    {
    private:
        friend struct ThreadLocalStorage;
        ThreadCache() = default;

        MemoryPool* allocatePool(const size_t objSize) override
        {
            //@TODO: 对CentralCache的支持
            //auto* slot = static_cast<char*>(std::malloc(objSize * EACH_POOL_SLOT_NUM));
            char* slot = static_cast<char*>(CentralCache::getCache()->allocate(objSize * EACH_POOL_SLOT_NUM));
            if (slot == nullptr)
                return nullptr;
            const auto pool = new MemoryPool(slot, objSize * EACH_POOL_SLOT_NUM, objSize);
            pools[objSize / ALIGN - 1]->addPool(pool);
            return pool;
        }

        void deallocatePool(MemoryPool* pool) override
        {
            //@TODO: 对CentralCache的支持
            //std::free(pool->getFirstPtr());
            CentralCache::getCache()->deallocate(pool->getFirstPtr(), pool->getPoolSize());
            delete pool;
        }

    public:
        constexpr static size_t mul = 1;
        ThreadCache(const ThreadCache&) = delete;
        ThreadCache& operator=(const ThreadCache&) = delete;
        ~ThreadCache() override { cleanup(); }

        void* allocate(const size_t objSize) override
        {
            // 如果大于最大内存池大小，则直接从PageCache中分配
            if (objSize > MAX_SLOT_SIZE)
            {
                return PageCache::getCache()->allocate(objSize);
            }
            return CacheBase::allocate(objSize);
        }

        void deallocate(void* ptr, const size_t objSize) override
        {
            // 如果大于最大内存池大小，则直接释放到PageCache中
            if (objSize > MAX_SLOT_SIZE)
            {
                PageCache::getCache()->deallocate(ptr, objSize);
            }
            CacheBase::deallocate(ptr, objSize);
        }
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_THREADCACHE_H
