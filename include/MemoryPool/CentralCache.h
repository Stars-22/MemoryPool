//
// Created by LinJun on 2025/9/23.
//

#ifndef CENTRALCACHE_H
#define CENTRALCACHE_H

#include <array>
#include "../../include/config.h"
#include "MemoryPools.h"

namespace MemoryPool
{
    // 中央缓存类，管理不同大小的内存池
    class CentralCache
    {
    private:
        std::array<MemoryPools_Lock*, MAX_SLOT_SIZE / ALIGN> pools; // 按块大小分组的内存池数组
        CentralCache();
        ~CentralCache();

    public:
        CentralCache(const CentralCache&) = delete;
        CentralCache& operator=(const CentralCache&) = delete;

        /**
         * @brief 获取当前线程的缓存实例
         * @return CentralCache指针
         */
        static CentralCache* getCache();
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

#endif // CENTRALCACHE_H
