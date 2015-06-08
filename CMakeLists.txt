cmake_minimum_required(VERSION 3.2)
project(Cache_Project)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    cache.h
    cache_list.h
    low_cache.h
    RAND_strategy.c
    random.h
    strategy.h
    tst_Cache.c)

add_executable(Cache_Project ${SOURCE_FILES})