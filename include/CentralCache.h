//
// Created by LinJun on 2025/9/23.
//

#ifndef CENTRALCACHE_H
#define CENTRALCACHE_H

#include <MemoryPool.h>
#include <array>
#include <mutex>

namespace MemoryPool
{
    // 桶结构体，用于管理特定大小的内存池
    struct Bucket
    {
        MemoryPool* mp; // 指向内存池对象的指针
        std::mutex mutex;           // 互斥锁，保证线程安全访问
    };

    // 中央缓存类，管理不同大小的内存池
    class CentralCache
    {
    private:
        static CentralCache* cache;
        // 构造函数：初始化所有桶的内存池指针为空
        CentralCache() : buckets()
        {
            // 遍历并初始化所有桶
            for (auto& item : buckets)
            {
                item.mp = nullptr; // 初始状态无内存池
            }
        }


    public:
        static CentralCache* getCache();

        // 向页缓存请求新页面
        void requestPageFromPageCache(size_t size);

        // 释放页面回页缓存
        void releasePageToPageCache();

        // 分配指定大小的内存池
        MemoryPool* allocate(size_t slot_size);

        // 从线程缓存接收内存池
        void receiveMemoryPoolFromThreadCache(MemoryPool* mp);

        // 公开的桶数组（实际应用中通常设为private）
        std::array<Bucket, 9> buckets; // 9个桶，管理不同大小的内存池
    };
}

#endif // CENTRALCACHE_H
