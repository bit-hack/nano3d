file(GLOB SOURCE_FILES source/*.cpp)
file(GLOB HEADER_FILES source/*.h)

include_directories(../nano3d/)

add_library(nano3d_ex ${SOURCE_FILES} ${HEADER_FILES} nano3d_ex.h ../nano3d/nano3d.h)
target_include_directories(nano3d_ex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(nano3d_ex PROPERTIES
    FOLDER nano3d_ex
)
