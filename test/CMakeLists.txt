project(test)

add_executable(test main.cpp properties.rc)
target_include_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/engine/include/)
target_link_libraries(test engine -static)

set_target_properties(
        test PROPERTIES
        OUTPUT_NAME "Vixen Engine Test Application"
)