# ---- BUILD TEST ----

find_package(Threads REQUIRED)

add_executable(
        test
        main.cpp
        properties.rc
)

target_include_directories(
        test
        PUBLIC
        ${CMAKE_SOURCE_DIR}/engine/include
)
if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_libraries(test -mwindows)
endif (CMAKE_BUILD_TYPE MATCHES "Release")
if (WIN32)
    target_link_libraries(
            test
            VixenEngine
            Threads::Threads
    )
else (WIN32)
    target_link_libraries(
            test
            VixenEngine
            -static-libstdc++
            -static-libgcc
    )
endif (WIN32)

set_target_properties(
        test PROPERTIES
        OUTPUT_NAME "Vixen Engine Test Application"
)