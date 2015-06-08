cmake_minimum_required(VERSION 3.2)

project(cache)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Cache_Project/cache.h
    Cache_Project/cache_list.h
    Cache_Project/Cache_Project.pdf
    Cache_Project/cache_size-inv.sh
    Cache_Project/libCache.a
    Cache_Project/low_cache.h
    Cache_Project/Makefile
    Cache_Project/RAND_strategy.c
    Cache_Project/random.h
    Cache_Project/strategy.h
    Cache_Project/tst_Cache.c)

add_executable(cache ${SOURCE_FILES})

