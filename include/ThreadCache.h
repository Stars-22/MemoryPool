//
// Created by stars on 2025/9/4.
//

#ifndef MEMORYPOOL_THREADCACHE_H
#define MEMORYPOOL_THREADCACHE_H
#include <map>
#include <array>
#include "MemoryPool.h"
#include "Config.h"

namespace MemoryPool
{
    /**
     * @brief 线程缓存类，用于管理线程本地的内存池
     */
    class ThreadCache
    {
    private:
        /**
         * @brief 内存池结构体，用于存储和管理内存池
         */
        struct MemoryPools
        {
            std::map<char*, MemoryPool*> poolsMap;  // 存储内存池的映射表，键为内存地址，值为内存池指针
            MemoryPool* firstPool;                  // 指向第一个内存池的指针

            /**
             * @brief 构造函数，初始化firstPool为nullptr
             */
            MemoryPools() : firstPool(nullptr)
            {
            }

            /**
             * @brief 析构函数，释放所有内存池资源
             */
            ~MemoryPools()
            {
                for (auto& pair : poolsMap)
                {
                    // @TODO 此处是测试ThreadCache代码
                    deallocatePool(pair.second->getFirstPtr(), pair.second->getPoolSize());
                    delete pair.second;  // 删除内存池
                }
                poolsMap.clear();      // 清空映射表
                firstPool = nullptr;   // 重置指针
            }
        };

        static thread_local ThreadCache* cache;  // 线程本地的ThreadCache指针
        std::array<MemoryPools*, MAX_SLOT_SIZE / ALIGN> pools;  // 内存池数组，按大小分类存储
        ThreadCache();  // 私有构造函数

        /**
         * @brief 分配一个新的内存池
         * @param slotSize 内存槽的大小
         * @return 返回新分配的内存池指针
         */
        static MemoryPool* allocatePool(size_t slotSize);

        /**
         * @brief 释放内存池
         * @param ptr 要释放的内存地址
         * @param objSize 要释放的内存大小
         */
        static void deallocatePool(void* ptr, size_t objSize);

        static void* allocateFromPageCache(size_t objSize);

        static void deallocateToPageCache(void* ptr, size_t objSize);

    public:
        ThreadCache(const ThreadCache&) = delete;              // 禁用拷贝构造
        ThreadCache& operator=(const ThreadCache&) = delete;   // 禁用赋值操作
        ~ThreadCache();                                       // 析构函数

        /**
         * @brief 获取当前线程的缓存实例
         * @return 返回线程本地的ThreadCache指针
         */
        static ThreadCache* getCache();

        /**
         * @brief 分配内存
         * @param objSize 要分配的对象大小
         * @return 返回分配的内存指针
         */
        static void* allocate(size_t objSize);

        /**
         * @brief 释放内存
         * @param ptr 要释放的内存指针
         * @param objSize 对象的大小
         */
        static void deallocate(void* ptr, size_t objSize);
    };
} // MemoryPool

#endif //MEMORYPOOL_THREADCACHE_H
