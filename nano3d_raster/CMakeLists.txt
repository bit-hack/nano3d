file(GLOB SOURCE_FILES source/*.cpp)
file(GLOB HEADER_FILES source/*.h)

include_directories(../nano3d/)

add_library(nano3d_raster ${SOURCE_FILES} ${HEADER_FILES} ../nano3d/nano3d.h)
target_include_directories(nano3d_raster PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
