//
// Created by stars on 2025/10/6.
//

#include "../../include/MemoryPool/PageCache.h"
#include <cstdlib>
#include "MemoryPool/SpansController.h"

namespace MemoryPool
{
    PageCache* PageCache::cache = new PageCache();

    PageCache::~PageCache()
    {
        //@TODO: 这里是否需要析构？
        // for (size_t i = 0; i < MAX_PAGE_NUM; i++)
        // {
        //     Span* temp = spans[i].first;
        //     while (temp != nullptr)
        //     {
        //         Span* span = temp;
        //         temp = temp->next;
        //         deallocate(span, (i + 1) * EACH_PAGE_SIZE);
        //     }
        // }
    }

    PageCache* PageCache::getCache() { return cache; }

    void* PageCache::allocate(size_t size)
    {
        std::lock_guard lock(mutex_);
        size_t pageNum = (size - 1) / EACH_PAGE_SIZE;
        if (pageNum > MAX_PAGE_NUM)
        {
            return allocateFromSystem(pageNum);
        }
        void* ptr = spansController.get(size);
        if (ptr == nullptr)
        {
            ptr = spansController.addAndCut(allocateFromSystem(), size);
        }
        return ptr;
    }

    void PageCache::deallocate(void* ptr, size_t objSize)
    {
        std::lock_guard lock(mutex_);
        objSize = (objSize + EACH_PAGE_SIZE - 1) / EACH_PAGE_SIZE * EACH_PAGE_SIZE;
        if (objSize >= EACH_PAGE_SIZE * MAX_PAGE_NUM)
        {
            deallocateToSystem(ptr);
            return;
        }
        if (ptr = spansController.rtn(ptr, objSize); ptr != nullptr)
        {
            deallocateToSystem(ptr);
        }
    }

    void* PageCache::allocateFromSystem(size_t pageNum)
    {
        //@OPTIMIZE:页对齐分配内存
        return std::malloc(EACH_PAGE_SIZE * pageNum);
    }

    void PageCache::deallocateToSystem(void* ptr) { free(ptr); }
} // namespace MemoryPool
