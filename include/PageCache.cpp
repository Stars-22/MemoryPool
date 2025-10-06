//
// Created by stars on 2025/10/6.
//

#include "PageCache.h"

#include <assert.h>
#include <cstring>

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
        return span;
    }

    void PageCache::deallocate(void* ptr, size_t objSize)
    {
        assert(objSize % EACH_PAGE_SIZE == 0);
        auto* span = static_cast<char*>(ptr);
        if (span_head.count(span + objSize) > 0)
        {
            Span* span_temp = span_head[span + objSize];
            size_t spanSize_right = span_temp->size;
            span_temp->~Span();
            objSize += spanSize_right;
        }
        if (span_tail.count(span) > 0)
        {
            Span* span_temp = span_tail[span];
            size_t spanSize_Left = span_temp->size;
            span_temp->~Span();
            objSize += spanSize_Left;
            span = static_cast<char*>(static_cast<void*>(span_temp));
        }
        if (objSize == MAX_PAGE_NUM * EACH_PAGE_SIZE)
        {
            deallocateToSystem(span);
            return;
        }
        memset(ptr, 0, objSize);
        spans[objSize / EACH_PAGE_SIZE - 1].add(new (span) Span(objSize));
    }
} // namespace MemoryPool
