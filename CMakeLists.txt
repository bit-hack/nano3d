cmake_minimum_required(VERSION 2.8)

message(STATUS ${CMAKE_CXX_COMPILER_ID})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project (nano3d)

add_subdirectory(nano3d)
add_subdirectory(nano3d_raster)

add_subdirectory(tests)
add_subdirectory(examples)
