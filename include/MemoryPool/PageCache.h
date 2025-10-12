//
// Created by stars on 2025/10/6.
//

#ifndef MEMORYPOOL_PAGECACHE_H
#define MEMORYPOOL_PAGECACHE_H

#include <array>
#include <unordered_map>
#include "../config.h"

namespace MemoryPool
{

    class PageCache
    {
    private:
        struct Span
        {
            size_t size;
            Span* prev = nullptr;
            Span* next = nullptr;
            explicit Span(const size_t size) : size(size) {}
            ~Span();
        };
        struct Spans
        {
            Span* first = nullptr;
            Span* last = nullptr;
            Span* get();
            void add(Span* span);
        };
        friend struct Span;

        static PageCache* cache;
        std::array<Spans, MAX_PAGE_NUM> spans;
        std::unordered_map<void*, Span*> spans_head;
        std::unordered_map<void*, Span*> spans_tail;

        PageCache() {}
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
