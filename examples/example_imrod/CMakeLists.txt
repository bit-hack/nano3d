file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

include_directories(${SDL_INCLUDE_DIR})

add_executable(example_imrod ${SOURCE_FILES} ${HEADER_FILES} ../models/imrod.cpp)
target_link_libraries(example_imrod LINK_PUBLIC nano3d nano3d_ex ${SDL_LIBRARY})

set_target_properties(example_imrod PROPERTIES
    FOLDER examples
)
