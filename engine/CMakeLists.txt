project(engine)

# ---- BUILD GLFW ----

set(GLFW contrib/glfw/)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
set(BUILD_SHARED_LIBS ON CACHE BOOL "")

add_subdirectory(${GLFW})

# ---- BUILD ENGINE ----

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

add_library(engine SHARED
        src/Window.cpp
        src/Instance.cpp
        src/Device.cpp)

if (VIXEN_DEBUG)
    target_compile_definitions(engine PUBLIC VIXEN_DEBUG)
    if (VIXEN_DEBUG_LEVEL MATCHES "VERBOSE")
        target_compile_definitions(engine PUBLIC VIXEN_DEBUG_LEVEL=0)
    elseif (VIXEN_DEBUG_LEVEL MATCHES "INFO")
        target_compile_definitions(engine PUBLIC VIXEN_DEBUG_LEVEL=1)
    elseif (VIXEN_DEBUG_LEVEL MATCHES "WARNING")
        target_compile_definitions(engine PUBLIC VIXEN_DEBUG_LEVEL=2)
    elseif (VIXEN_DEBUG_LEVEL MATCHES "ERROR")
        target_compile_definitions(engine PUBLIC VIXEN_DEBUG_LEVEL=3)
    endif ()
endif (VIXEN_DEBUG)

set_target_properties(engine PROPERTIES PREFIX "" OUTPUT_NAME "vixenengine")

target_include_directories(engine PUBLIC include/ ${Vulkan_INCLUDE_DIRS} ${GLFW}/include/ contrib/glm/)
target_link_libraries(engine Threads::Threads glfw Vulkan::Vulkan)