file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

include_directories(${SDL_INCLUDE_DIR})

add_executable(glbunny ${SOURCE_FILES} ${HEADER_FILES} ../../examples/models/bunny.cpp)
target_link_libraries(glbunny LINK_PUBLIC nano3d nano3d_ex ${SDL_LIBRARY} OPENGL32.lib)

set_target_properties(glbunny PROPERTIES
    FOLDER tests
)
