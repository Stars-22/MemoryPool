//
// Created by stars on 2025/10/6.
//

#include "../include/PageCache.h"

#include <cstdlib>
#include <cstring>

namespace MemoryPool
{
    PageCache* PageCache::cache = new PageCache();

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

    PageCache* PageCache::getCache() { return cache; }

    void* PageCache::allocate(size_t size)
    {
        size_t pageNum = (size - 1) / EACH_PAGE_SIZE;
        if (pageNum > MAX_PAGE_NUM)
        {
            return allocateFromSystem(pageNum);
        }
        if (spans[pageNum].first != nullptr)
        {
            void* ptr = spans[pageNum].get();
            memset(ptr, 0, EACH_PAGE_SIZE * (pageNum + 1));
            return ptr;
        }
        void* span = nullptr;
        size_t spanSize = 0;
        for (size_t i = pageNum; i < MAX_PAGE_NUM; i++)
        {
            if (spans[i].first == nullptr)
                continue;
            span = spans[i].get();
            spanSize = static_cast<Span*>(span)->size;
            spans_tail.erase(static_cast<char*>(span) + spanSize);
            break;
        }
        if (span == nullptr)
        {
            span = allocateFromSystem();
            spanSize = MAX_PAGE_NUM * EACH_PAGE_SIZE;
            auto* span_Span = new (span) Span(spanSize);
            spans_head.insert(std::make_pair(span, span_Span));
        }
        void* spanReturn = nullptr;
        spanSize = spanSize - size;
        spanReturn = static_cast<char*>(span) + spanSize;
        if (span == spanReturn)
        {
            spans_head.erase(span);
            return spanReturn;
        }
        static_cast<Span*>(span)->size = spanSize;
        spans_tail.insert(std::make_pair(static_cast<char*>(span) + spanSize, static_cast<Span*>(span)));
        spans[spanSize / EACH_PAGE_SIZE - 1].add(static_cast<Span*>(span));
        return spanReturn;
    }

    void PageCache::deallocate(void* ptr, size_t objSize)
    {
        objSize = (objSize + EACH_PAGE_SIZE - 1) / EACH_PAGE_SIZE * EACH_PAGE_SIZE;
        if (objSize > EACH_PAGE_SIZE * MAX_PAGE_NUM)
        {
            deallocateToSystem(ptr);
            return;
        }
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
        spans_tail.insert(std::make_pair(span_tail, span));
    }

    void* PageCache::allocateFromSystem(size_t pageNum)
    {
        //@OPTIMIZE:页对齐分配内存
        return std::malloc(EACH_PAGE_SIZE * pageNum);
    }

    void PageCache::deallocateToSystem(void* ptr) { free(ptr); }
} // namespace MemoryPool
