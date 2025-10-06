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
    struct Span
    {
        size_t size;
        Span* prev = nullptr;
        Span* next = nullptr;
        Span(size_t size) : size(size) {}
        ~Span();
    };

    class PageCache
    {
    private:
        static thread_local PageCache* cache;
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
        std::unordered_map<void*, Span*> span_head;
        std::unordered_map<void*, Span*> span_tail;
        friend struct Span;

    public:
        PageCache(const PageCache&) = delete;
        PageCache& operator=(const PageCache&) = delete;
        ~PageCache();
        static PageCache* getCache();
        void* allocate(size_t size);
        void deallocate(void* ptr, size_t objSize);
        void* allocateFromSystem();
        void deallocateToSystem(void* ptr);
    };

} // namespace MemoryPool

#endif // MEMORYPOOL_PAGECACHE_H
