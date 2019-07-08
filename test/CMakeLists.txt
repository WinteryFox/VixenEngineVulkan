project(test)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(VIXEN_DEBUG=1)
endif (CMAKE_BUILD_TYPE MATCHES "Debug")

generate_product_version(
        VersionFilesOutputVariable
        NAME "Vixen Engine Test Application"
        ICON "${CMAKE_SOURCE_DIR}/icon.ico"
        VERSION_MAJOR 1
        VERSION_MINOR 0
        VERSION_PATH 0
        VERSION_REVISION 0
        COMPANY_NAME "Vixen Studio"
        ORIGINAL_FILENAME "Vixen Engine Test Application.exe"
        FILE_DESCRIPTION "Test application for Vixen Engine"
)

add_executable(test main.cpp ${VersionFilesOutputVariable})
target_include_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/engine/include/)
target_link_libraries(test engine)
set_target_properties(test PROPERTIES OUTPUT_NAME "Vixen Engine Test Application")

if (WIN32)
    target_link_libraries(test gdi32 user32 kernel32)
endif (WIN32)