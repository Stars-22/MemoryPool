cmake_minimum_required(VERSION 4.0)

project(MemoryPool)

add_executable(MemoryPool main.cpp
        ThreadCache.h
        ThreadCache.cpp
        MemoryPool.cpp
        MemoryPool.h
        Config.h
)
