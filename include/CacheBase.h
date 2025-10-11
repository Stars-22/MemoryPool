//
// Created by stars on 2025/10/11.
//

#ifndef MEMORYPOOL_CACHEBASE_H
#define MEMORYPOOL_CACHEBASE_H
#include <array>
#include <cassert>
#include "MemoryPool.h"
#include "config.h"

namespace MemoryPool
{
    // 单例存储策略
    struct GlobalStorage
    {
        template <typename T>
        static T* getCache()
        {
            static T cache;
            return &cache;
        }
    };
    struct ThreadLocalStorage
    {
        template <typename T>
        static T* getCache()
        {
            thread_local T cache;
            return &cache;
        }
    };

    template <typename Pools, typename Derived, typename StoragePolicy = GlobalStorage>
    class CacheBase
    {
    protected:
        constexpr static size_t mul = Derived::mul;
        std::array<Pools*, MAX_SLOT_SIZE / ALIGN> pools; // 按块大小分组的内存池数组
        CacheBase() : pools{}
        {
            for (auto& poolGroup : pools)
            {
                poolGroup = new Pools();
            }
        }

        /**
         * @brief 分配一个新的内存池
         * @param objSize 所需内存块的大小
         * @return 返回新分配的内存池指针
         */
        virtual MemoryPool* allocatePool(size_t objSize) = 0;

        /**
         * @brief 释放内存池
         * @param pool 要释放的内存池
         */
        virtual void deallocatePool(MemoryPool* pool) = 0;

        void cleanup()
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

    public:
        CacheBase(const CacheBase&) = delete;
        CacheBase& operator=(const CacheBase&) = delete;
        virtual ~CacheBase() = default;


        /**
         * @brief 获取当前线程的缓存实例
         * @return 线程本地ThreadCache指针
         */
        static Derived* getCache() { return StoragePolicy::template getCache<Derived>(); } // namespace MemoryPool

        /**
         * @brief 分配指定大小的内存
         * @param objSize 要分配的字节数
         * @return 分配的内存指针，失败返回nullptr
         */
        virtual void* allocate(size_t objSize)
        {
            const size_t slotNum = (objSize + ALIGN * mul - 1) / (ALIGN * mul) - 1;
            const size_t slotSize = (slotNum + 1) * (ALIGN * mul);
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

        /**
         * @brief 释放内存
         * @param ptr 要释放的内存指针
         * @param objSize 内存大小
         */
        virtual void deallocate(void* ptr, size_t objSize)
        {
            assert(ptr != nullptr && "ptr is nullptr");
            const size_t slotNum = (objSize + (ALIGN * mul) - 1) / (ALIGN * mul) - 1;
            MemoryPool* pool = pools[slotNum]->deallocate(ptr);
            if (pools[slotNum]->deallocate(ptr))
            {
                // 内存池已空，释放资源
                deallocatePool(pool);
            }
        }
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_CACHEBASE_H
