cmake_minimum_required(VERSION 3.14)

project(PoolAllocator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(test)

set(POOLALLOCATOR_FILES 
    pool_allocator_port_api.h
    pool_allocator_port.h 
    pool_allocator.h
    singelton/singelton.h)

include(CTest)