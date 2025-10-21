//
// Created by stars on 2025/10/6.
//

#include "../../include/MemoryPool/PageCache.h"
#include <cstdlib>
#include "MemoryPool/SpansController.h"

namespace MemoryPool
{

    PageCache* PageCache::getCache()
    {
        static PageCache cache;
        return &cache;
    }

    void* PageCache::allocate(size_t size)
    {
        std::lock_guard lock(mutex_);
        size_t pageNum = (size - 1) / EACH_PAGE_SIZE;
        if (pageNum > MAX_PAGE_NUM)
        {
            return allocateFromSystem(size);
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
        if (objSize % EACH_PAGE_SIZE != 0)
        {
            objSize = (objSize + EACH_PAGE_SIZE) / EACH_PAGE_SIZE * EACH_PAGE_SIZE;
        }
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

    void* PageCache::allocateFromSystem(size_t size)
    {
        //@OPTIMIZE:页对齐分配内存
        void* block = std::malloc(size);
        if (block) {
            system_blocks.insert(block);  // 记录分配的块
        }
        return block;
    }

    void PageCache::deallocateToSystem(void* ptr)
    {
        system_blocks.erase(ptr);
        free(ptr);
    }

    bool PageCache::isHead(void* ptr) const { return system_blocks.count(ptr) > 0; }
} // namespace MemoryPool
