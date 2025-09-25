#include <iostream>
#include "../include/allocate.h"

/**
 * @brief 测试用结构体
 */
struct S1
{
    int a;
    int b;
    int c;

    S1(const int a, const int b, const int c) : a(a), b(b), c(c) {}
    S1() = default;
};

std::ostream& operator<<(std::ostream& os, const S1& s)
{
    os << "S{a='" << s.a << "', b=" << s.b << ", c=" << s.c << "}";
    return os;
}

/**
 * 主函数，演示内存池的使用
 */
int main()
{

    int* a = allocate<int>(100000000);
    std::cout << *a << std::endl;

    int* c = allocate<int>();
    *c = 10;
    std::cout << *c << std::endl;

    int* d[10];
    for (int i = 0; i < 10; i++)
    {
        d[i] = allocate<int>(i);
    }

    // 从内存池分配一个S1对象大小的空间，并使用构造函数参数初始化
    S1* b = allocate<S1>(100, 101, 102);
    std::cout << sizeof(*b) << std::endl;
    std::cout << *b << std::endl;

    // 输出三个指针的地址
    std::cout << a << " " << c << " " << b << std::endl;
    // 计算并输出c和a之间的字节差（即它们之间的偏移量）
    std::cout << reinterpret_cast<char*>(c) - reinterpret_cast<char*>(a) << std::endl;
    for (auto& i : d)
    {
        std::cout << *i << " " << i << std::endl;
    }

    return 0;
}
