#include "../include/CentralCache.h"

#include <stdlib.h>
size_t list[] = {8, 16, 24, 32, 40, 48, 56, 64,72};

void CentralCache::requestPageToPageCache(size_t size) {
    char *page_begin = static_cast<char *>(malloc(size));
    char* tmp = page_begin;
    for(int i=0;i<9;i++) {
        size_t s = size / 360 * list[i];
        auto pool = new MemoryPool::MemoryPool(tmp, s, list[i]);
        if(buckets[i].mp == nullptr) {
            buckets[i].mp = pool;
            pool->nextPool = pool;
            pool->prevPool = pool;
        }else {
            auto head = buckets[i].mp;
            auto tail = head->prevPool;
            head->prevPool = pool;
            tail->nextPool = pool;
            pool->prevPool = tail;
            pool->nextPool = head;
        }
            tmp+=s;
    }
}

void CentralCache::releasePageToPageCache() {
}

MemoryPool::MemoryPool * CentralCache::allocate(size_t slot_size) {
    size_t size = 0;
    for(auto& i : list) {
        if (i >= slot_size) {
            size = i;
            break;
        }
    }
    size_t num = size / 8 - 1;
    std::lock_guard<std::mutex> lock(buckets[num].mutex);
    auto head = buckets[num].mp;
    for(auto tmp=head;tmp->nextPool!=head;tmp=tmp->nextPool) {
        if(tmp->getUsedAmount() != 0) {
            continue;
        }else {
            return tmp;
        }
    }
    releasePageToPageCache();
    return head->prevPool;
}

void CentralCache::receiveMemoryPoolFromThreadCache(MemoryPool::MemoryPool *mp) {
    // 假设size 是对应的
    size_t num = mp->getSlotSize() / 8 - 1;
    buckets[num].mutex.lock();
    if(!buckets[num].mp) {
        buckets[num].mp = mp;
        mp->nextPool = mp;
        mp->nextPool = mp;
    }else {
        auto head = buckets[num].mp;
        auto tail = head->prevPool;
        mp->prevPool = tail;
        mp->nextPool = head;
        head->prevPool = mp;
        tail->nextPool = mp;
    }
}
