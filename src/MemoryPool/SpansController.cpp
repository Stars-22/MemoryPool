//
// Created by stars on 2025/10/14.
//

#include "../../include/MemoryPool/SpansController.h"
#include "MemoryPool/PageCache.h"

namespace MemoryPool
{

    SpansController::Span::~Span()
    {
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
    }

    SpansController::Span* SpansController::Spans::getFirst() const { return first; }

    void SpansController::Spans::add(Span* span)
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

    void SpansController::Spans::remove(const Span* span)
    {
        if (span == first) first = span->next;
        if (span == last) last = span->prev;
        if (span->prev) span->prev->next = span->next;
        if (span->next) span->next->prev = span->prev;
    }

    void SpansController::add(void* ptr, size_t size)
    {
        auto* span = new (ptr) Span(size);
        if (!PageCache::getCache()->isHead(ptr)) spans_head[ptr] = span;
        spans_tail[static_cast<char*>(ptr) + size] = span;
        spans[size / EACH_PAGE_SIZE - 1].add(span);
    }

    void SpansController::remove(void* span)
    {
        const size_t spansSize = static_cast<Span*>(span)->size;
        spans_head.erase(span);
        spans_tail.erase(static_cast<char*>(span) + spansSize);
        spans[spansSize / EACH_PAGE_SIZE - 1].remove(static_cast<Span*>(span));
    }

    SpansController::Span* SpansController::getFindHead(void* ptr)
    {
        if (spans_head.count(ptr) > 0)
        {
            Span* span = spans_head[ptr];
            remove(span);
            return span;
        }
        return nullptr;
    }

    SpansController::Span* SpansController::getFindTail(void* ptr)
    {
        if (spans_tail.count(ptr) > 0)
        {
            Span* span = spans_tail[ptr];
            remove(span);
            return span;
        }
        return nullptr;
    }

    void* SpansController::cut(Span* span, size_t size)
    {
        spans[span->size / EACH_PAGE_SIZE - 1].remove(span);
        spans_tail.erase(span + span->size);
        span->size -= size;
        void* span_tailPtr = static_cast<char*>(static_cast<void*>(span)) + span->size;
        spans_tail[span_tailPtr] = span;
        spans[span->size / EACH_PAGE_SIZE - 1].add(span);
        return span_tailPtr;
    }

    void* SpansController::rtn(void* ptr, size_t size)
    {
        char* ptr_char = static_cast<char*>(ptr);
        if (!PageCache::getCache()->isHead(ptr))
        {
            if (Span* span_left = getFindTail(ptr); span_left != nullptr)
            {
                size += span_left->size;
                ptr = span_left;
                span_left->~Span();
            }
        }
        if (Span* span_right = getFindHead(ptr_char + size); span_right != nullptr)
        {
            size += span_right->size;
            span_right->~Span();
        }
        if (size == MAX_PAGE_NUM * EACH_PAGE_SIZE)
        {
            return ptr;
        }
        add(ptr, size);
        return nullptr;
    }

    void* SpansController::get(size_t size)
    {
        size_t pageNum = size / EACH_PAGE_SIZE - 1;
        if (Span* span = spans[pageNum].getFirst(); span != nullptr)
        {
            remove(span);
            span->~Span();
            return span;
        }
        for (size_t i = pageNum + 1; i <= MAX_PAGE_NUM; i++)
        {
            if (spans[pageNum].getFirst() != nullptr)
            {
                return cut(spans[pageNum].getFirst(), size);
            }
        }
        return nullptr;
    }

    void* SpansController::addAndCut(void* ptr, size_t needSize, size_t totalSize)
    {
        size_t spanSize = totalSize - needSize;
        add(ptr, spanSize);
        return static_cast<char*>(ptr) + spanSize;
    }

} // namespace MemoryPool
