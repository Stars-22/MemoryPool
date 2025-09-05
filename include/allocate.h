//
// Created by stars on 2025/9/5.
//

#ifndef MEMORYPOOL_ALLOCATE_H
#define MEMORYPOOL_ALLOCATE_H
#include "ThreadCache.h"
// @TODO 智能指针接口

template <typename T, typename... Args>
T* allocate(Args&&... args)
{
    using namespace MemoryPool;
    T* ptr = static_cast<T*>(ThreadCache::allocate(sizeof(T)));
    if (ptr)
    {
        // 使用参数包直接构造对象
        new(ptr) T(std::forward<Args>(args)...);
    }
    return ptr;
}

template <typename T>
void deallocate(T* ptr)
{
    using namespace MemoryPool;
    assert(ptr != nullptr && "ptr is nullptr");
    ThreadCache::deallocate(ptr, sizeof(*ptr));
}

#endif //MEMORYPOOL_ALLOCATE_H