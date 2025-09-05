#include <iostream>
#include "../include/allocate.h"

/**
 * @brief 结构体S1的定义，包含三个整型成员变量和两个构造函数
 */
struct S1
{
    int a;
    int b;
    int c;

    S1(const int a, const int b, const int c) : a(a), b(b), c(c)
    {
    }

    S1() = default;
};

/**
 * @brief 重载S1结构体的输出流运算符<<
 * @param os 输出流对象，用于输出S1结构体的内容
 * @param s 要输出的S1结构体常量引用
 * @return 返回输出流对象的引用，以支持链式输出
 * 
 * 该函数将S1结构体的内容格式化为字符串并输出到指定的输出流中。
 * 输出格式为："S{a='a的值', b=b的值, c=c的值}"
 */
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
    // 使用MemoryPool命名空间
    using namespace MemoryPool;
    // 从内存池分配100000000个int大小的空间，但不初始化
    int* a = allocate<int>(100000000);
    //*a = 100000000;  // 这行被注释掉，未对a指向的内存进行赋值
    std::cout << *a << std::endl;  // 输出a指向的内存值（未初始化的随机值）

    // 从内存池分配一个int大小的空间
    int* c = allocate<int>();
    *c = 10;  // 对c指向的内存赋值为10
    std::cout << *c << std::endl;  // 输出c的值（10）

    int *d[10];
    for (int i=0; i<10; i++)
    {
        d[i] = allocate<int>(i);
    }

    // 从内存池分配一个S1对象大小的空间，并使用构造函数参数初始化
    S1* b = allocate<S1>(100, 101, 102);
    // 以下三行被注释掉，因为已经通过构造函数初始化了成员变量
    // b->a = 100;
    // b->b = 101;
    // b->c = 102;
    std::cout << sizeof(*b) << std::endl;  // 输出S1对象的大小
    std::cout << *b << std::endl;  // 输出S1对象的内容（需要重载<<运算符）

    // 输出三个指针的地址
    std::cout << a << " " << c << " " << b << std::endl;
    // 计算并输出c和a之间的字节差（即它们之间的偏移量）
    std::cout << reinterpret_cast<char*>(c) - reinterpret_cast<char*>(a) << std::endl;
    for (auto & i : d)
    {
        std::cout << *i << " " << i << std::endl;
    }

    return 0;  // 程序正常退出
}
