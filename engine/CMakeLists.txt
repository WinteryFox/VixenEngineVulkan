project(engine)

# ---- BUILD ENGINE ----

set(
        vixen_HEADERS
        include/Window.h
        include/Instance.h
        include/Logger.h
        include/PhysicalDevice.h
        include/LogicalDevice.h
)
set(
        vixen_SOURCES
        properties.rc
        src/Window.cpp
        src/Instance.cpp
        src/PhysicalDevice.cpp
        src/LogicalDevice.cpp
)

add_library(
        engine
        SHARED
        ${vixen_HEADERS}
        ${vixen_SOURCES}
)

set_target_properties(
        engine PROPERTIES
        PREFIX ""
        OUTPUT_NAME "VixenEngine"
)

find_package(PNG REQUIRED)
find_package(GLFW REQUIRED)
find_package(Vulkan REQUIRED)

target_include_directories(
        engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIR}
        ${GLFW_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/glm/
        ${PNG_INCLUDE_DIR}
)
target_link_libraries(
        engine
        Vulkan::Vulkan
        ${GLFW_LIBRARIES}
        PNG::PNG
        -static
)