/**
 * @file config.h
 * @brief 内存池配置参数定义
 */

#ifndef MEMORYPOOL_CONFIG_H
#define MEMORYPOOL_CONFIG_H

#include <cstddef>

namespace MemoryPool
{
    constexpr size_t ALIGN = 8;               // 内存对齐字节数
    constexpr size_t MAX_SLOT_SIZE = 1024;    // 内存池管理的最大块大小
    constexpr size_t EACH_POOL_SLOT_NUM = 16; // 每个内存池包含的块数量


    constexpr size_t EACH_PAGE_SIZE = 4096; // 单个页块的大小
    constexpr size_t MAX_PAGE_NUM = 128;    // 最大页数量（单次向系统申请内存的页数量）
} // namespace MemoryPool

#endif // MEMORYPOOL_CONFIG_H
