project(editor)

set(IMGUI_DIR contrib/imgui)
set(
        IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)

add_executable(editor main.cpp ${IMGUI_SRC})
target_link_libraries(editor engine)
target_include_directories(editor PUBLIC ../engine/src contrib/imgui)
