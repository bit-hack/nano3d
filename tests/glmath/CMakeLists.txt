file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

include_directories(${SDL_INCLUDE_DIR})

add_executable(glmath ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(glmath LINK_PUBLIC nano3d nano3d_ex ${SDL_LIBRARY} OPENGL32.lib)

set_target_properties(glmath PROPERTIES
    FOLDER tests
)
