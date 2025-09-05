//
// Created by stars on 2025/9/4.
//

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H
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

        static MemoryPool* allocatePool(size_t slotSize);

        static void deallocatePool(MemoryPool* pool);

        static void* allocate(size_t objSize);

        static void deallocate(void* ptr, size_t objSize);
    };
} // MemoryPool

#endif //MEMORYPOOL_THREADCACHE_H