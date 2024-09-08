cmake_minimum_required(VERSION 3.27)

project(Test CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/singelton)
include("${CMAKE_SOURCE_DIR}/test/googletest.cmake")

add_executable(${PROJECT_NAME} test.cpp)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    GTest::GTest gtest gmock
    Singelton
    PoolAllocator
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})