# ---- BUILD TEST ----

find_package(Threads REQUIRED)

add_executable(
        VixenEngineTest
        main.cpp
        properties.rc
)

target_include_directories(
        VixenEngineTest
        PUBLIC
        ${CMAKE_SOURCE_DIR}/engine/include
)
if (CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_libraries(VixenEngineTest -mwindows)
endif (CMAKE_BUILD_TYPE MATCHES "Release")
if (WIN32)
    target_link_libraries(
            VixenEngineTest
            VixenEngine
            Threads::Threads
    )
else (WIN32)
    target_link_libraries(
            VixenEngineTest
            VixenEngine
            -static-libstdc++
            -static-libgcc
    )
endif (WIN32)

set_target_properties(
        VixenEngineTest PROPERTIES
        OUTPUT_NAME "Vixen Engine Test Application"
)