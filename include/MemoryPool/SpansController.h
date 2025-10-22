//
// Created by stars on 2025/10/14.
//

#ifndef MEMORYPOOL_SPANS_H
#define MEMORYPOOL_SPANS_H

#include <array>
#include <unordered_map>
#include "../config.h"

namespace MemoryPool
{

    class SpansController
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
        class Spans
        {
        private:
            Span* first = nullptr;
            Span* last = nullptr;

        public:
            [[nodiscard]] Span* getFirst() const;
            void add(Span* span);
            void remove(const Span* span);
        };

        std::array<Spans, MAX_PAGE_NUM> spans;
        std::unordered_map<void*, Span*> spans_head;
        std::unordered_map<void*, Span*> spans_tail;

        Span* getFindHead(void* ptr);
        Span* getFindTail(void* ptr);
        void* cut(Span* span, size_t size);

    public:
        void add(void* ptr, size_t size);
        void remove(void* span);
        void* get(size_t size);
        void* rtn(void* ptr, size_t size);
        void* addAndCut(void* ptr, size_t needSize, size_t totalSize = MAX_PAGE_NUM * EACH_PAGE_SIZE);
    };


} // namespace MemoryPool

#endif // MEMORYPOOL_SPANS_H
