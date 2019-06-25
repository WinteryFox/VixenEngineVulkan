project(engine)

# ---- BUILD GLFW ----

set(GLFW contrib/glfw/)

if (VIXEN_WITH_GLFW)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
    set(GLFW_INSTALL OFF CACHE BOOL "")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "")

    add_subdirectory(${GLFW})
endif (VIXEN_WITH_GLFW)

# ---- BUILD ENGINE ----

set(
        vixen_HEADERS
        include/Window.h
        include/Instance.h
        include/Logger.h
)
set(
        vixen_SOURCES
        src/Window.cpp
        src/Instance.cpp
)

add_library(
        engine
        SHARED
        ${vixen_HEADERS}
        ${vixen_SOURCES}
)

set_target_properties(engine PROPERTIES PREFIX "" OUTPUT_NAME "VixenEngine")

if (NOT VIXEN_WITH_GLFW)
    find_package(GLFW REQUIRED)
endif (NOT VIXEN_WITH_GLFW)
find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

target_include_directories(engine PUBLIC include contrib/log4cpp/include ${Vulkan_INCLUDE_DIR} ${GLFW}/include/ contrib/glm/)
target_link_libraries(engine Threads::Threads Vulkan::Vulkan glfw)