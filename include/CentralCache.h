//
// Created by LinJun on 2025/9/10.
//

#ifndef CENTRALCACHE_H
#define CENTRALCACHE_H
#include <array>
#include <memory>

#include <mutex>
#include "MemoryPool.h"
// span是一个双链表节点，每个节点中有一个内存池。
class Span
{
public:
    Span* next;
    Span* prev;
    MemoryPool::MemoryPool* mp;
    Span() : Span(0, nullptr, nullptr, nullptr) {}; // 直接委托给单参数构造函数
    Span(int id) : Span(id, nullptr, nullptr, nullptr) {};
    Span(int id, MemoryPool::MemoryPool* mp, Span* next, Span* prev) : page_id(id), mp{mp}, next{next}, prev{prev} {};
    Span(int id, char* mp_start, size_t slot_size, size_t pool_size, Span* next, Span* prev) :
        Span(id, new MemoryPool::MemoryPool{mp_start, pool_size, slot_size}, next, prev) {};
    bool isExhaust() const
    {
        // 内存池的内存用完了返回False，没用返回Ture
        return mp->getSlotAmount() >= mp->getUsedAmount();
    }
    void* allocate() { return mp->allocate(); }

private:
    int page_id;
};
// Spanlist是一个span双链表，并且带锁，
// 可以分配内存，释放内存，通过节点中内存池操作

class Spanlist
{
public:
    Spanlist()
    {
        head = new Span;
        head->next = head;
        head->prev = head;
        span_num = 0;
    }
    Span* getFirstSpan() const { return firstSpan; }
    Span* getFreeSpan() const { return freeSpan; }

    void* allocate(size_t size)
    {
        mutex.lock();
        void* ptr;
        if (freeSpan == nullptr)
        {
            ptr = firstSpan->allocate();
        }
        else
        {
            ptr = freeSpan->allocate();
        }
        mutex.unlock();
        return ptr;
    }

private:
    Span* head;
    int span_num;
    Span* firstSpan; // 一个span指针，指向目前可以分配内存的第一个span
    Span* freeSpan; // 一个span指针，指向目前被释放的span节点。
public:
    std::mutex mutex;
};
class CentralCache
{
private:
    std::array<Spanlist*, 10> span_array_;

public:
    void* allocate(size_t obj_size)
    {
        int span_num = sizeof(obj_size);
        auto sl = span_array_[span_num];
        return sl->allocate(obj_size);
    }
};
#endif // CENTRALCACHE_H
