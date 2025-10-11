//
// Created by stars on 2025/10/11.
//

#ifndef MEMORYPOOL_MEMORYPOOLS_H
#define MEMORYPOOL_MEMORYPOOLS_H

#include <MemoryPool.h>
#include <cassert>
#include <map>
#include <mutex>

namespace MemoryPool
{

    /**
     * @brief 内存池容器，管理同一大小的多个内存池
     */
    struct MemoryPools
    {
        std::map<char*, MemoryPool*> poolsMap; // 内存地址到内存池的映射
        MemoryPool* firstPool;                 // 内存池链表头

        MemoryPools() : firstPool(nullptr) {}

        ~MemoryPools()
        {
            poolsMap.clear();
            firstPool = nullptr;
        }

        [[nodiscard]] void* allocate() const
        {
            MemoryPool* pool = firstPool;
            if (pool != nullptr)
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

        MemoryPool* deallocate(void* ptr)
        {
            auto it = poolsMap.lower_bound(static_cast<char*>(ptr));
            assert(it != poolsMap.end() && "pool is nullptr");

            MemoryPool* pool = it->second;
            if (pool->deallocate(ptr))
            {
                if (pool != firstPool)
                {
                    pool->prevPool->nextPool = pool->nextPool;
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

        void addPool(MemoryPool* pool)
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
    };

    /**
     * @brief 内存池容器（包含互斥锁），管理同一大小的多个内存池
     */
    struct MemoryPools_Lock : MemoryPools
    {
        void* allocate()
        {
            std::lock_guard lock(mutex_);
            return MemoryPools::allocate();
        }

        MemoryPool* deallocate(void* ptr)
        {
            std::lock_guard lock(mutex_);
            return MemoryPools::deallocate(ptr);
        }

        void addPool(MemoryPool* pool)
        {
            std::lock_guard lock(mutex_);
            MemoryPools::addPool(pool);
        }

    private:
        std::mutex mutex_;
    };

} // namespace MemoryPool

#endif // MEMORYPOOL_MEMORYPOOLS_H
