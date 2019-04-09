project(engine)

# ---- BUILD GLFW ----

set(GLFW contrib/glfw/)

add_subdirectory(${GLFW})

# ---- BUILD ENGINE ----

find_package(Vulkan REQUIRED)

add_library(engine SHARED
        src/Window.cpp
        include/Window.h src/Instance.cpp include/Instance.h src/Device.cpp include/Device.h)
target_include_directories(engine PUBLIC include/ ${Vulkan_INCLUDE_DIRS} ${GLFW}/include/ contrib/glm/)
target_link_libraries(engine glfw ${Vulkan_LIBRARIES})