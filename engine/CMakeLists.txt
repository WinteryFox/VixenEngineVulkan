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

add_library(engine SHARED
        src/Window.cpp
        src/Instance.cpp
        src/Device.cpp)

set_target_properties(engine PROPERTIES PREFIX "" OUTPUT_NAME "vixenengine")

target_include_directories(engine PUBLIC include/ ${Vulkan_INCLUDE_DIRS} ${GLFW}/include/ contrib/glm/)
target_link_libraries(engine glfw ${Vulkan_LIBRARIES})