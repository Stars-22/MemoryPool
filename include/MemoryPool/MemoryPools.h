//
// Created by stars on 2025/10/11.
//

#ifndef MEMORYPOOL_MEMORYPOOLS_H
#define MEMORYPOOL_MEMORYPOOLS_H

#include <map>
#include <mutex>
#include "MemoryPool.h"

namespace MemoryPool
{
    /**
     * @brief 内存池容器，管理同一大小的多个内存池
     */
    struct MemoryPools
    {
        std::map<char*, MemoryPool*> poolsMap; // 内存地址到内存池的映射
        MemoryPool* firstPool = nullptr;       // 内存池链表头
        size_t poolSize;
        explicit MemoryPools(size_t size);
        ~MemoryPools();

        [[nodiscard]] void* allocate() const;
        MemoryPool* deallocate(void* ptr);
        void addPool(MemoryPool* pool);
    };

    /**
     * @brief 内存池容器（包含互斥锁），管理同一大小的多个内存池
     */
    struct MemoryPools_Lock : MemoryPools
    {
        explicit MemoryPools_Lock(size_t size);
        void* allocate();
        void deallocate(void* ptr);
        void addPool(MemoryPool* pool);
    private:
        std::mutex mutex_;
        MemoryPool* allocatePool();
        static void deallocatePool(const MemoryPool* pool);
    };

} // namespace MemoryPool

#endif // MEMORYPOOL_MEMORYPOOLS_H
