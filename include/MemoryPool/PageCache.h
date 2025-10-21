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
        std::mutex mutex_;
        SpansController spansController;

        PageCache() = default;
        ~PageCache();
        static void* allocateFromSystem(size_t size = MAX_PAGE_NUM * EACH_PAGE_SIZE);
        static void deallocateToSystem(void* ptr);

    public:
        PageCache(const PageCache&) = delete;
        PageCache& operator=(const PageCache&) = delete;

        static PageCache* getCache();
        void* allocate(size_t size);
        void deallocate(void* ptr, size_t objSize);
    };

} // namespace MemoryPool

#endif // MEMORYPOOL_PAGECACHE_H
