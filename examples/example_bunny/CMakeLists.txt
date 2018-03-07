file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

include_directories(${SDL_INCLUDE_DIR})

add_executable(example_bunny ${SOURCE_FILES} ${HEADER_FILES} ../models/bunny.cpp)
target_link_libraries(example_bunny LINK_PUBLIC nano3d nano3d_ex ${SDL_LIBRARY})

set_target_properties(example_bunny PROPERTIES
    FOLDER examples
)
