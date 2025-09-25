//
// Created by LinJun on 2025/9/23.
//

#ifndef CENTRALCACHE_H
#define CENTRALCACHE_H
#include <array>
#include <MemoryPool.h>
#include <mutex>

struct Bucket {
    MemoryPool::MemoryPool* mp;
    std::mutex mutex;
};
class CentralCache {
public:
    CentralCache():buckets() {
        for (auto& item : buckets) {
            item.mp = nullptr;
        }
    }
    void requestPageToPageCache(size_t size);
    void releasePageToPageCache();
    MemoryPool::MemoryPool *allocate(size_t slot_size);
    void receiveMemoryPoolFromThreadCache(MemoryPool::MemoryPool *mp);

//private:
    std::array<Bucket,9> buckets;
};



#endif //CENTRALCACHE_H
