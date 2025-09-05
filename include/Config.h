//
// Created by stars on 2025/9/4.
//

#ifndef MEMORYPOOL_CONFIG_H
#define MEMORYPOOL_CONFIG_H

namespace MemoryPool{
    const size_t ALIGN = 8;                //内存池对齐值
    const size_t MAX_SLOTSIZE = 512;       //内存池中最大块大小
    const size_t EACHPOOL_SLOT_NUM = 10;   //每个内存池中块的数量
}

#endif // MEMORYPOOL_CONFIG_H