file(GLOB SOURCE_FILES source/*.cpp)
file(GLOB HEADER_FILES source/*.h)

source_group(api FILES nano3d.h)

add_library(nano3d ${SOURCE_FILES} ${HEADER_FILES} nano3d.h)
target_include_directories(nano3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(nano3d PROPERTIES
    FOLDER nano3d
)
