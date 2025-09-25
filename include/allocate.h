/**
 * @file allocate.h
 * @brief 内存池分配器接口，提供类型安全的内存分配和释放功能
 */

#ifndef MEMORYPOOL_ALLOCATE_H
#define MEMORYPOOL_ALLOCATE_H
#include "ThreadCache.h"
// @TODO 智能指针接口

/**
 * @brief 从内存池分配并构造单个对象
 * @tparam T 要分配的对象类型
 * @tparam Args 构造函数参数类型包
 * @param args 传递给构造函数的参数
 * @return 指向新构造对象的指针，失败时返回nullptr
 */
template <typename T, typename... Args>
T* allocate(Args&&... args)
{
    using namespace MemoryPool;
    T* ptr = static_cast<T*>(ThreadCache::allocate(sizeof(T)));
    if (ptr)
    {
        new (ptr) T(std::forward<Args>(args)...);
    }
    return ptr;
}

/**
 * @brief 析构并释放单个对象回内存池
 * @tparam T 要释放的对象类型
 * @param ptr 指向要释放对象的指针
 */
template <typename T>
void deallocate(T* ptr)
{
    using namespace MemoryPool;
    assert(ptr != nullptr && "ptr is nullptr");
    if constexpr (!std::is_trivially_destructible_v<T>)
    {
        ptr->~T();
    }
    ThreadCache::deallocate(ptr, sizeof(*ptr));
}

/**
 * @brief 从内存池分配并构造对象数组
 * @tparam T 数组元素类型
 * @param size 数组大小
 * @return 指向数组首元素的指针，失败时返回nullptr
 * @note 对于字符类型会自动添加null终止符
 */
template <typename T>
T* allocateArray(const size_t size)
{
    using namespace MemoryPool;
    size_t totalSize = sizeof(T) * size + sizeof(size_t);

    if (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
    {
        totalSize += sizeof(T);
    }

    void* ptr_void = ThreadCache::allocate(totalSize);
    auto* ptr_size = static_cast<size_t*>(ptr_void);
    T* ptr = static_cast<T*>(static_cast<void*>(static_cast<char*>(ptr_void) + sizeof(size_t)));

    *ptr_size = totalSize;

    if constexpr (!std::is_trivially_default_constructible_v<T>)
    {
        for (size_t i = 0; i < size; ++i)
        {
            new (&ptr[i]) T();
        }
    }

    if (std::is_same_v<T, char> || std::is_same_v<T, signed char> || std::is_same_v<T, unsigned char>)
    {
        ptr[size] = T(0);
    }
    return ptr;
}

/**
 * @brief 释放数组内存回内存池
 * @param ptr 指向数组首元素的指针
 */
inline void deallocateArray(void* ptr)
{
    using namespace MemoryPool;
    void* ptr_void = static_cast<char*>(ptr) - sizeof(size_t);
    const size_t totalSize = *static_cast<size_t*>(ptr_void);

    ThreadCache::deallocate(ptr_void, totalSize);
}

#endif // MEMORYPOOL_ALLOCATE_H
