/**
 * @file ThreadCache.h
 * @brief 线程本地内存缓存管理器
 */

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H
#include <array>
#include <map>
#include "Config.h"
#include "MemoryPool.h"

namespace MemoryPool
{
    /**
     * @brief 线程缓存类，用于管理线程本地的内存池
     */
    class ThreadCache
    {
    private:
        /**
         * @brief 内存池容器，管理同一大小的多个内存池
         */
        struct MemoryPools
        {
            std::map<char*, MemoryPool*> poolsMap; // 内存地址到内存池的映射
            MemoryPool* firstPool;                 // 内存池链表头

            MemoryPools() : firstPool(nullptr) {}

            ~MemoryPools()
            {
                for (auto& pair : poolsMap)
                {
                    // @TODO 此处是测试ThreadCache代码
                    deallocatePool(pair.second->getFirstPtr(), pair.second->getPoolSize());
                    delete pair.second;
                }
                poolsMap.clear();
                firstPool = nullptr;
            }
        };

        static thread_local ThreadCache* cache;                // 线程本地缓存实例
        std::array<MemoryPools*, MAX_SLOT_SIZE / ALIGN> pools; // 按块大小分组的内存池数组
        ThreadCache();

        /**
         * @brief 分配一个新的内存池
         * @param objSize 所需内存块的大小
         * @return 返回新分配的内存池指针
         */
        static MemoryPool* allocatePool(size_t objSize);

        /**
         * @brief 释放内存池
         * @param ptr 要释放的内存地址
         * @param objSize 要释放的内存大小
         */
        static void deallocatePool(void* ptr, size_t objSize);

        /**
         * @brief 从页面缓存（Page Cache）中分配指定大小的内存块
         * @param objSize 所需内存块的大小
         * @return void* 返回新分配内存的指针
         */
        static void* allocateFromPageCache(size_t objSize);

        /**
         * @brief 将内存块释放回页面缓存（Page Cache）
         * @param ptr 要释放的内存地址
         * @param objSize 释放的内存大小
         * @return void
         */
        static void deallocateToPageCache(void* ptr, size_t objSize);

    public:
        ThreadCache(const ThreadCache&) = delete;
        ThreadCache& operator=(const ThreadCache&) = delete;
        ~ThreadCache();

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
        static void* allocate(size_t objSize);

        /**
         * @brief 释放内存
         * @param ptr 要释放的内存指针
         * @param objSize 内存大小
         */
        static void deallocate(void* ptr, size_t objSize);
    };
} // namespace MemoryPool

#endif // MEMORYPOOL_THREADCACHE_H
