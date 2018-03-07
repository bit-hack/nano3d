file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES *.h)

add_executable(test_unit ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(test_unit LINK_PUBLIC nano3d)

set_target_properties(test_unit PROPERTIES
    FOLDER tests
)
