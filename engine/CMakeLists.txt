# ---- BUILD ENGINE ----

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/contrib/FBXImport)

set(
        vixen_LIBRARY
        Vulkan::Vulkan
        Threads::Threads
        FBXImport
)
set(
        vixen_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/FBXImport/importer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/VMA/src
)

if (vixen_BUILD_GLFW)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/contrib/glfw)
    list(APPEND vixen_LIBRARY glfw)
    list(APPEND vixen_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glfw/include)
else (vixen_BUILD_GLFW)
    find_package(GLFW REQUIRED)
    list(APPEND vixen_LIBRARY ${GLFW_LIBRARY})
    list(APPEND vixen_INCLUDE ${GLFW_INCLUDE})
endif (vixen_BUILD_GLFW)

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
        include/Scene.h
        include/Camera.h
        include/Input.h
        include/VertexShader.h
        include/FragmentShader.h
        include/MeshStore.h)
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
        src/Input.cpp
)

add_library(
        VixenEngine
        SHARED
        ${vixen_SOURCES}
        ${vixen_HEADERS}
)

target_include_directories(
        VixenEngine
        PUBLIC
        ${vixen_INCLUDE}
)

target_link_libraries(
        VixenEngine
        ${vixen_LIBRARY}
)

set_target_properties(
        VixenEngine PROPERTIES
        PREFIX ""
        OUTPUT_NAME "VixenEngine"
)