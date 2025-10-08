//
// Created by stars on 2025/10/6.
//

#include "PageCache.h"

#include <assert.h>
#include <cstring>
#include <cstdlib>

namespace MemoryPool
{
    Span::~Span()
    {
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
        if (!prev && !next)
        {
            PageCache::getCache()->spans[size / EACH_PAGE_SIZE - 1].first = nullptr;
        }
    }

    PageCache::~PageCache()
    {
        for (size_t i = 0; i < MAX_PAGE_NUM; i++)
        {
            Span* temp = spans[i].first;
            while (temp != nullptr)
            {
                Span* span = temp;
                temp = temp->next;
                deallocate(span, (i + 1) * EACH_PAGE_SIZE);
            }
        }
    }

    PageCache* PageCache::getCache()
    {
        if (cache == nullptr)
        {
            cache = new PageCache();
        }
        return cache;
    }

    void* PageCache::allocate(size_t size)
    {
        assert(size % EACH_PAGE_SIZE == 0);
        size_t pageNum = size / EACH_PAGE_SIZE - 1;
        if (spans[pageNum].first != nullptr)
        {
            void* ptr = spans[pageNum].get();
            memset(ptr, 0, EACH_PAGE_SIZE * (pageNum + 1));
            return ptr;
        }
        void* span = nullptr;
        for (size_t i = pageNum; i < MAX_PAGE_NUM; i++)
        {
            if (spans[i].first == nullptr)
                continue;
            span = spans[i].get();
            break;
        }
        if (span == nullptr)
        {
            span = allocateFromSystem();
        }
        size_t spanNeedlessSize = (pageNum + 1) * EACH_PAGE_SIZE;
        auto* spanNeedless = new (static_cast<char*>(span) + spanNeedlessSize) Span(spanNeedlessSize);
        spans[MAX_PAGE_NUM - pageNum].add(spanNeedless);
        spans_head.insert(std::make_pair(spanNeedless, spanNeedless));
        void* spanNeedless_tail = static_cast<char*>(static_cast<void*>(spanNeedless)) + spanNeedlessSize;
        spans_tail.insert(std::make_pair(spanNeedless_tail , spanNeedless));
        return span;
    }

    void PageCache::deallocate(void* ptr, size_t objSize)
    {
        assert(objSize % EACH_PAGE_SIZE == 0);
        auto* span_ptr = static_cast<char*>(ptr);
        if (spans_head.count(span_ptr + objSize) > 0)
        {
            Span* span_temp = spans_head[span_ptr + objSize];
            spans_tail.erase(span_ptr + objSize);
            size_t spanSize_right = span_temp->size;
            span_temp->~Span();
            objSize += spanSize_right;
        }
        if (spans_tail.count(span_ptr) > 0)
        {
            Span* span_temp = spans_tail[span_ptr];
            spans_tail.erase(span_ptr);
            size_t spanSize_Left = span_temp->size;
            span_temp->~Span();
            objSize += spanSize_Left;
            span_ptr = static_cast<char*>(static_cast<void*>(span_temp));
        }
        if (objSize == MAX_PAGE_NUM * EACH_PAGE_SIZE)
        {
            deallocateToSystem(span_ptr);
            return;
        }
        memset(ptr, 0, objSize);
        Span* span = new (span_ptr) Span(objSize);
        spans[objSize / EACH_PAGE_SIZE - 1].add(span);
        spans_head.insert(std::make_pair(span, span));
        void* span_tail = static_cast<char*>(static_cast<void*>(span)) + objSize;
        spans_tail.insert(std::make_pair(span_tail , span));
    }

    void* PageCache::allocateFromSystem()
    {
        //@OPTIMIZE:页对齐分配内存
        return std::malloc(EACH_PAGE_SIZE * MAX_PAGE_NUM);
    }

    void PageCache::deallocateToSystem(void* ptr)
    {
        free(ptr);
    }
} // namespace MemoryPool
