/**
 * @file ThreadCache.h
 * @brief 线程本地内存缓存管理器
 */

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H
#include <array>

#include "PageCache.h"
#include "CacheBase.h"
#include "MemoryPools.h"
#include "config.h"

namespace MemoryPool
{
    /**
     * @brief 线程缓存类，用于管理线程本地的内存池
     */
    class ThreadCache final : public CacheBase<MemoryPools, ThreadCache, ThreadLocalStorage>
    {
    private:
        friend struct ThreadLocalStorage;
        ThreadCache() = default;

        MemoryPool* allocatePool(const size_t objSize) override
        {
            //@TODO: 对CentralCache的支持
            auto* slot = static_cast<char*>(std::malloc(objSize * EACH_POOL_SLOT_NUM));
            // char* slot = CentralCache::getCache()->allocate(slotSize * EACH_POOL_SLOT_NUM);
            if (slot == nullptr)
                return nullptr;

            const auto pool = new MemoryPool(slot, objSize * EACH_POOL_SLOT_NUM, objSize);
            pools[objSize / ALIGN - 1]->addPool(pool);
            return pool;
        }

        void deallocatePool(MemoryPool* pool) override
        {
            //@TODO: 对CentralCache的支持
            std::free(pool->getFirstPtr());
            // CentralCache::getCache()->deallocate(pool->getFirstPtr(), pool->getPoolSize());
            delete pool;
        }

    public:
        ThreadCache(const ThreadCache&) = delete;
        ThreadCache& operator=(const ThreadCache&) = delete;
        ~ThreadCache() override
        {
            for (const auto& poolGroup : pools)
            {
                for (auto& pair : poolGroup->poolsMap)
                {
                    //@TODO: 此处是测试ThreadCache代码
                    deallocatePool(pair.second);
                }
                delete poolGroup;
            }
        }

        void* allocate(const size_t objSize) override
        {
            // 如果大于最大内存池大小，则直接从PageCache中分配
            if (objSize > MAX_SLOT_SIZE)
            {
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

        void deallocate(void* ptr, const size_t objSize) override
        {
            // 如果大于最大内存池大小，则直接释放到PageCache中
            if (objSize > MAX_SLOT_SIZE)
            {
                PageCache::getCache()->deallocate(ptr, objSize);
            }
            assert(ptr != nullptr && "ptr is nullptr");

            const size_t slotNum = (objSize + ALIGN - 1) / ALIGN - 1;
            MemoryPool* pool = pools[slotNum]->deallocate(ptr);
            if (pools[slotNum]->deallocate(ptr))
            {
                // 内存池已空，释放资源
                deallocatePool(pool);
            }
        }
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_THREADCACHE_H
