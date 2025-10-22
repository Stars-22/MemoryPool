/**
 * @file ThreadCache.h
 * @brief 线程本地内存缓存管理器
 */

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H

#include <array>
#include "../../include/config.h"
#include "MemoryPools.h"

namespace MemoryPool
{
    // 线程缓存类，用于管理线程本地的内存池
    class ThreadCache
    {
    private:
        std::array<MemoryPools*, MAX_SLOT_SIZE / ALIGN> pools; // 按块大小分组的内存池数组
        ThreadCache();
        ~ThreadCache();

        /**
         * @brief 分配一个新的内存池
         * @param objSize 所需内存块的大小
         * @return 返回新分配的内存池指针
         */
        [[nodiscard]] MemoryPool* allocatePool(size_t objSize) const;
        /**
         * @brief 释放内存池
         * @param pool 要释放的内存池
         */
        static void deallocatePool(const MemoryPool* pool);

    public:
        ThreadCache(const ThreadCache&) = delete;
        ThreadCache& operator=(const ThreadCache&) = delete;

        /**
         * @brief 获取当前线程的缓存实例
         * @return 线程本地ThreadCache指针
         */
        static ThreadCache* getCache();
        /**
         * @brief 分配指定大小的内存
         * @param objSize 要分配的字节数
         * @return 分配的内存指针，失败返回nullptr
         */
        [[nodiscard]] void* allocate(size_t objSize) const;
        /**
         * @brief 释放内存
         * @param ptr 要释放的内存指针
         * @param objSize 内存大小
         */
        void deallocate(void* ptr, size_t objSize) const;
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_THREADCACHE_H
