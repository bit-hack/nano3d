cmake_minimum_required(VERSION 3.1)

message(STATUS ${CMAKE_CXX_COMPILER_ID})

if( ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

project (nano3d)

add_subdirectory(nano3d)
add_subdirectory(nano3d_raster)

add_subdirectory(tests)
add_subdirectory(examples)
