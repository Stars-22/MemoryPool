//
// Created by stars on 2025/9/4.
//

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H
#include <bits/std_thread.h>
#include <map>
#include <array>
#include "MemoryPool.h"
#include "Config.h"

namespace MemoryPool
{
    class ThreadCache
    {
    private:
        struct MemoryPools{
            std::map<char*, MemoryPool*> poolsmap;
            MemoryPool* firstPool;
            MemoryPools() : firstPool(nullptr) {}
            ~MemoryPools(){
                for (auto& pair : poolsmap) {
                    delete pair.second;
                }
                poolsmap.clear();
                firstPool = nullptr;
            }
        };
        static thread_local ThreadCache* cache;
        std::array<MemoryPools*, MAX_SLOTSIZE/ALIGN> pools;
        ThreadCache();
    public:
        ThreadCache(const ThreadCache&) = delete;
        ThreadCache& operator=(const ThreadCache&) = delete;
        ~ThreadCache();

        static ThreadCache* getCache();

        template <typename T>
        static T* allocate(){
            // 计算需要的槽大小，通过将类型T的大小向上取整到8的倍数并除以8
            size_t slotNum = (sizeof(T) + ALIGN - 1) / ALIGN - 1;
            size_t slotSize = (slotNum + 1) * ALIGN;
            // 从缓存中获取对应槽大小的内存池
            MemoryPool* pool = getCache()->pools[slotNum]->firstPool;
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
        static void deallocate(T* ptr);

        static MemoryPool* allocatePool(size_t slotSize);

        static void deallocatePool(MemoryPool* pool);
    };
} // MemoryPool

#endif //MEMORYPOOL_THREADCACHE_H