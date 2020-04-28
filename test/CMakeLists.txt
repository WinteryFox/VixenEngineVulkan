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

# Remove console in release windows build
if (WIN32 AND CMAKE_BUILD_TYPE MATCHES "Release")
    target_link_libraries(VixenEngineTest -mwindows)
endif (WIN32 AND CMAKE_BUILD_TYPE MATCHES "Release")

# Compile shaders
add_custom_target(
        vert glsLangValidator -V test.vert -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vert.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)
add_custom_target(
        frag glsLangValidator -V test.frag -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/frag.spv
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)
add_dependencies(VixenEngineTest vert frag)

target_link_libraries(
        VixenEngineTest
        VixenEngine
        Threads::Threads
)

set_target_properties(
        VixenEngineTest PROPERTIES
        OUTPUT_NAME "Vixen Engine Test Application"
)