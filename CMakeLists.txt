cmake_minimum_required(VERSION 3.10)
project(MemoryPool)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 包含头文件目录
include_directories(include)

# 收集所有源文件
file(GLOB_RECURSE SOURCES
        "src/*.cpp"
)

# 创建可执行文件
add_executable(MemoryPool ${SOURCES})

# 设置目标属性
target_include_directories(MemoryPool PRIVATE include)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(MemoryPool PRIVATE -Wall -Wextra -pedantic)
endif()