//
// Created by stars on 2025/10/6.
//

#ifndef MEMORYPOOL_PAGECACHE_H
#define MEMORYPOOL_PAGECACHE_H

#include <array>
#include <unordered_map>
#include "config.h"

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
            Span(size_t size) : size(size) {}
            ~Span()
            {
                if (prev)
                    prev->next = next;
                if (next)
                    next->prev = prev;
                if (!prev && !next)
                {
                    getCache()->spans[size / EACH_PAGE_SIZE - 1].first = nullptr;
                }
            }
        };

        static PageCache* cache;
        PageCache(){}

        struct Spans
        {
            Span* first = nullptr;
            Span* last = nullptr;
            Span* get()
            {
                Span* span = first;
                if (first != nullptr)
                {
                    first = first->next;
                    first->prev = nullptr;
                }
                return span;
            }
            void add(Span* span)
            {
                if (first == nullptr)
                    first = span;
                else
                {
                    span->prev = last;
                    last->next = span;
                }
                last = span;
            }
        };
        std::array<Spans, MAX_PAGE_NUM> spans;
        std::unordered_map<void*, Span*> spans_head;
        std::unordered_map<void*, Span*> spans_tail;
        friend struct Span;

        void* allocateFromSystem(size_t pageNum = MAX_PAGE_NUM);
        void deallocateToSystem(void* ptr);

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
