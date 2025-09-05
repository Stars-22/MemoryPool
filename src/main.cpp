#include <iostream>
#include "../include/allocate.h"

struct S1
{
    int a;
    int b;
    int c;

    S1(int a, int b, int c) : a(a), b(b), c(c)
    {
    }

    S1()
    {
    }
};

std::ostream& operator<<(std::ostream& os, const S1& s)
{
    os << "S{a='" << s.a << "', b=" << s.b << ", c=" << s.c << "}";
    return os;
}

int main()
{
    using namespace MemoryPool;
    int* a = allocate<int>(100000000);
    //*a = 100000000;
    std::cout << *a << std::endl;

    int* c = allocate<int>();
    *c = 10;
    std::cout << *c << std::endl;

    S1* b = allocate<S1>(100, 101, 102);
    // b->a = 100;
    // b->b = 101;
    // b->c = 102;
    std::cout << sizeof(*b) << std::endl;
    std::cout << *b << std::endl;

    std::cout << a << " " << c << " " << b << std::endl;
    std::cout << (char*)c - (char*)a << std::endl;

    return 0;
}
