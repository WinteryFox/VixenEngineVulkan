# ---- BUILD ENGINE ----

find_package(Vulkan REQUIRED)
find_package(GLFW REQUIRED)
find_package(Threads REQUIRED)

set(
        vixen_HEADERS
        include/Window.h
        include/Instance.h
        include/Logger.h
        include/PhysicalDevice.h
        include/LogicalDevice.h
        include/Shader.h
        include/Render.h
        include/Mesh.h
        include/Loader.h
        include/VixenEngine.h
        include/Entity.h
        include/Scene.h include/Camera.h)
set(
        vixen_SOURCES
        properties.rc
        src/Window.cpp
        src/Instance.cpp
        src/PhysicalDevice.cpp
        src/LogicalDevice.cpp
        src/Shader.cpp
        src/Render.cpp
        src/Mesh.cpp
        include/Input.h src/Input.cpp)

add_library(
        VixenEngine
        SHARED
        ${vixen_SOURCES}
        ${vixen_HEADERS}
)

target_compile_options(VixenEngine PRIVATE -fno-exceptions -fno-rtti)

set_target_properties(
        VixenEngine PROPERTIES
        PREFIX ""
        OUTPUT_NAME "VixenEngine"
)

target_include_directories(
        VixenEngine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/VMA/src
)

if (WIN32)
    target_link_libraries(
            VixenEngine
            Vulkan::Vulkan
            ${GLFW_LIBRARIES}
            -static-libstdc++
            -static-libgcc
            -static
            Threads::Threads
            -dynamic
    )
else (WIN32)
    target_link_libraries(
            VixenEngine
            Vulkan::Vulkan
            ${GLFW_LIBRARIES}
            -static-libstdc++
            -static-libgcc
    )
endif (WIN32)