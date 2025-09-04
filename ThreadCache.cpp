//
// Created by stars on 2025/9/4.
//

#include "ThreadCache.h"

namespace MemoryPool
{
    thread_local ThreadCache* ThreadCache::cache = nullptr;

    ThreadCache::ThreadCache() {
    }

    ThreadCache::~ThreadCache() {
        // 清理所有内存池
        for (const auto& poolGroup : pools) {
            delete poolGroup;
        }
    }

    ThreadCache* ThreadCache::getCache() {
        if (cache == nullptr) {
            cache = new ThreadCache();
        }
        return cache;
    }

    template <typename T>
    /**
     * 分配内存空间用于存储类型为T的对象
     * @return 返回指向分配内存的指针，如果分配失败则返回nullptr
     */
    T* ThreadCache::allocate(){
        // 计算需要的槽大小，通过将类型T的大小向上取整到8的倍数并除以8
        size_t slotSize = (sizeof(T) + 7) / 8 - 1;
        // 从缓存中获取对应槽大小的内存池
        MemoryPool* pool = getCache()->pools[slotSize]->firstPool;
        // 用于存储分配对象的指针
        void* obj = nullptr;
        // 尝试从当前内存池分配空间
        if(pool != nullptr){
            if ((obj = pool->allocate()) != nullptr) return (T*)obj;
            // 如果当前内存池分配失败，则尝试后续的内存池
            while(pool->nextPool != nullptr){
                if ((obj = pool->allocate()) != nullptr) return (T*)obj;
                pool = pool->nextPool;
            }
        }
        // 如果所有现有内存池都分配失败，则请求一个新的内存池
        pool = allocatePool(slotSize);
        // 尝试从新分配的内存池中分配空间
        if ((obj = pool->allocate()) != nullptr) return (T*)obj;
        // 如果所有尝试都失败，返回nullptr
        return nullptr;
    }

    template <typename T>
    void ThreadCache::deallocate(T* ptr) {
        assert(ptr != nullptr && "ptr is nullptr");
        // 计算槽大小，通过将对象大小向上取整到8的倍数并除以8
        size_t slotSize = (sizeof(*ptr) + 7) / 8 - 1;
        ptr = (char*)ptr;
        MemoryPools* pools = getCache()->pools[slotSize];
        // 查找所在的内存池
        auto it = pools->poolsmap.lower_bound(ptr);
        assert(it != pools->poolsmap.end() && "pool is nullptr");
        // 将对象指针返回给内存池
        MemoryPool* pool = it->second;
        if(pool->deallocate(ptr)){
            // 如果内存池已已清空，则释放此内存池
            deallocatePool(pool);
            if (pool != pools->firstPool)
            {
                pool->prevPool->nextPool = pool->nextPool;
                pool->nextPool->prevPool = pool->prevPool;
            }
            else
            {
                pools->firstPool = pool->nextPool;
            }
            // 从映射中移除
            pools->poolsmap.erase(it);
        }
    }

    MemoryPool* ThreadCache::allocatePool(size_t slotSize) {}

    void ThreadCache::deallocatePool(MemoryPool* pool) {}

} // MemoryPool