file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

include_directories(${SDL_INCLUDE_DIR})

add_executable(example3 ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(example3 LINK_PUBLIC nano3d ${SDL_LIBRARY})
