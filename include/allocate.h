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
    // 如果T是非平凡可析构的，则调用析构函数
    if constexpr (!std::is_trivially_destructible_v<T>) {
        ptr->~T();
    }
    ThreadCache::deallocate(ptr, sizeof(*ptr));
}

template <typename T>
T* allocateArray(const size_t size)
{
    using namespace MemoryPool;
    size_t totalSize = sizeof(T) * size;

    if (std::is_same_v<T, char> ||
        std::is_same_v<T, signed char> ||
        std::is_same_v<T, unsigned char>)
    {
        totalSize += sizeof(T);
    }

    T* ptr = static_cast<T*>(ThreadCache::allocate(totalSize));

    if constexpr (!std::is_trivially_default_constructible_v<T>) {
        for (size_t i = 0; i < size; ++i) {
            new (&ptr[i]) T(); // 默认构造
        }
    }

    if (std::is_same_v<T, char> ||
        std::is_same_v<T, signed char> ||
        std::is_same_v<T, unsigned char>)
    {
        ptr[size] = T(0);
    }
    return ptr;
}

#endif //MEMORYPOOL_ALLOCATE_H