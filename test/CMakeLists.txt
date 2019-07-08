project(test)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(VIXEN_DEBUG=1)
endif (CMAKE_BUILD_TYPE MATCHES "Debug")

add_executable(test main.cpp properties.rc)
target_include_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/engine/include/)
target_link_libraries(test engine)
set_target_properties(test PROPERTIES OUTPUT_NAME "Vixen Engine Test Application")

if (WIN32)
    target_link_libraries(test gdi32 user32 kernel32)
endif (WIN32)