file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_executable(ut_threading ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(ut_threading LINK_PUBLIC nano3d)
