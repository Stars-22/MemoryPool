#include <iostream>
#include "ThreadCache.h"

struct S1 {
    int a;
    int b;
    int c;
};
std::ostream& operator<<(std::ostream& os, const S1& s) {
    os << "S{a='" << s.a << "', b=" << s.b << ", c=" << s.c << "}";
    return os;
}

int main()
{
    using namespace MemoryPool;
    int* a = ThreadCache::allocate<int>();
    *a = 100000000;
    std::cout << *a << std::endl;

    int* c = ThreadCache::allocate<int>();
    *c = 10;
    std::cout << *c << std::endl;

    S1* b = ThreadCache::allocate<S1>();
    b->a = 100;
    b->b = 101;
    b->c = 102;
    std::cout << sizeof(*b) << std::endl;
    std::cout << *b << std::endl;

    std::cout << a << " " << c <<" " << b << std::endl;
    std::cout << (char*)c - (char*)a << std::endl;

    return 0;
    
}