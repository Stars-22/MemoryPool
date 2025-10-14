//
// Created by stars on 2025/10/6.
//

#ifndef MEMORYPOOL_PAGECACHE_H
#define MEMORYPOOL_PAGECACHE_H

#include <mutex>
#include "../config.h"
#include "SpansController.h"

namespace MemoryPool
{

    class PageCache
    {
    private:
        static PageCache* cache;
        std::mutex mutex_;
        SpansController spansController;

        PageCache() = default;
        static void* allocateFromSystem(size_t pageNum = MAX_PAGE_NUM);
        static void deallocateToSystem(void* ptr);

    public:
        PageCache(const PageCache&) = delete;
        PageCache& operator=(const PageCache&) = delete;
        ~PageCache();

        static PageCache* getCache();
        void* allocate(size_t size);
        void deallocate(void* ptr, size_t objSize);
    };

} // namespace MemoryPool

#endif // MEMORYPOOL_PAGECACHE_H
