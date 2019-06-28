project(test)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(VIXEN_DEBUG=1)
endif (CMAKE_BUILD_TYPE MATCHES "Debug")

add_executable(test main.cpp)
target_include_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/engine/include/)
target_link_libraries(test engine)

if (WIN32)
    target_link_libraries(test gdi32 user32 kernel32)
endif (WIN32)