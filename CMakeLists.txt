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
file(GLOB_RECURSE COMMON_SOURCES
        "src/*.cpp"
        "src/*.c"
)

# 从公共源文件中排除main.cpp和test.cpp
list(FILTER COMMON_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
list(FILTER COMMON_SOURCES EXCLUDE REGEX ".*test\\.cpp$")

# 创建共享库（包含所有公共源文件）
add_library(MemoryPool_common STATIC ${COMMON_SOURCES})
target_include_directories(MemoryPool_common PUBLIC include)

# 创建主程序可执行文件
add_executable(MemoryPool_main src/main.cpp)
target_link_libraries(MemoryPool_main MemoryPool_common)

# 创建测试程序可执行文件
add_executable(MemoryPool_test src/test.cpp)
target_link_libraries(MemoryPool_test MemoryPool_common)

# 添加编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(MemoryPool_common PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(MemoryPool_main PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(MemoryPool_test PRIVATE -Wall -Wextra -pedantic)
endif()

## 可选：为测试目标添加DEBUG宏定义
#set_target_properties(MemoryPool_common PROPERTIES
#        COMPILE_DEFINITIONS "DEBUG"
#)