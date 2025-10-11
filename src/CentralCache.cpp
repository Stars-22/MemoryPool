#include "../include/CentralCache.h"
#include <cstdlib>

namespace MemoryPool
{
    CentralCache* CentralCache::cache = new CentralCache();
    // 每个内存池的槽大小
    size_t list[] = {8, 16, 24, 32, 40, 48, 56, 64, 72};

    CentralCache* CentralCache::getCache() { return cache; }

    void CentralCache::requestPageFromPageCache(size_t size)
    {
        // 获取内存
        char* page_begin = static_cast<char*>(malloc(size));
        char* tmp = page_begin;
        // 将获取的内存分割为内存池节点，挂载到原来的内存池链表中
        for (int i = 0; i < 9; i++)
        {
            // 为了均匀分割，每个内存池都有一样的槽数，360是list中的所有元素的大小。
            size_t s = size / 360 * list[i];
            auto pool = new MemoryPool(tmp, s, list[i]);
            if (buckets[i].mp == nullptr)
            {
                buckets[i].mp = pool;
                pool->nextPool = pool;
                pool->prevPool = pool;
            }
            else
            {
                auto head = buckets[i].mp;
                auto tail = head->prevPool;
                head->prevPool = pool;
                tail->nextPool = pool;
                pool->prevPool = tail;
                pool->nextPool = head;
            }
            tmp += s;
        }
    }

    void CentralCache::releasePageToPageCache() {}

    MemoryPool* CentralCache::allocate(size_t slot_size)
    {
        // 找到申请分配槽大小的内存池链表
        size_t size = 0;
        for (auto& i : list)
        {
            if (i >= slot_size)
            {
                size = i;
                break;
            }
        }
        size_t num = size / 8 - 1;
        std::lock_guard<std::mutex> lock(buckets[num].mutex);
        // 如果内存池链表中还有没有使用的内存池节点，返回
        auto head = buckets[num].mp;
        for (auto tmp = head; tmp->nextPool != head; tmp = tmp->nextPool)
        {
            if (tmp->getUsedAmount() != 0)
            {
                continue;
            }
            else
            {
                return tmp;
            }
        }
        // 如果没有新分配内存，返回
        releasePageToPageCache();
        return head->prevPool;
    }

    void CentralCache::receiveMemoryPoolFromThreadCache(MemoryPool* mp)
    {
        // 假设size 是对应的

        //@TODO: 返回的内存池的内部状态是被使用的，usedAmount ！= 0，私有属性无法改变。
        size_t num = mp->getSlotSize() / 8 - 1;
        buckets[num].mutex.lock();
        if (!buckets[num].mp)
        {
            buckets[num].mp = mp;
            mp->nextPool = mp;
            mp->nextPool = mp;
        }
        else
        {
            auto head = buckets[num].mp;
            auto tail = head->prevPool;
            mp->prevPool = tail;
            mp->nextPool = head;
            head->prevPool = mp;
            tail->nextPool = mp;
        }
    }

} // namespace MemoryPool
