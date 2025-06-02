
include(${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/versioning/versioning.cmake)

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(Boost ${FIND_PACKAGE_ARGS} CONFIG REQUIRED)
find_package(OpenGL ${FIND_PACKAGE_ARGS} REQUIRED)
find_package(GLEW ${FIND_PACKAGE_ARGS} REQUIRED)
find_package(glfw3 ${FIND_PACKAGE_ARGS} REQUIRED)
find_package(imgui ${FIND_PACKAGE_ARGS} CONFIG REQUIRED)
find_package(implot ${FIND_PACKAGE_ARGS} CONFIG REQUIRED)
find_package(fmt ${FIND_PACKAGE_ARGS} CONFIG REQUIRED)
find_package(nlohmann_json ${FIND_PACKAGE_ARGS} CONFIG REQUIRED)
find_package(tracy ${FIND_PACKAGE_ARGS} CONFIG REQUIRED)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILDTYPE)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_app.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_comp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_debug.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_exec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_filesystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_scope.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_stl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/iga_time.cpp
)

set(IMGUI_SOURCES
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/bindings/imgui_impl_glfw.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/bindings/imgui_impl_opengl3.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/misc/cpp/imgui_stdlib.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/src/imgui.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/src/imgui_demo.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/src/imgui_draw.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/src/imgui_tables.cpp
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/src/imgui_widgets.cpp
)

set(FONT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/fonts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/fonts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/font_opensans.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/font_robotomono.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/font_fa_regular.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/font_fa_solid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/fonts/font_fk_regular.cpp
)

add_library(imguiapp STATIC ${SOURCES} ${IMGUI_SOURCES} ${FONT_SOURCES})
target_link_libraries(imguiapp PUBLIC 
    glfw
    GLEW::GLEW
    OpenGL::GL
    imgui::imgui
    implot::implot
    
    Tracy::TracyClient
    opengl32
    
    Boost::boost
    fmt::fmt
    nlohmann_json::nlohmann_json
)

if (ENABLE_TRACY)
    target_compile_definitions(imguiapp PUBLIC
        TRACY_ENABLE
        TRACY_ON_DEMAND
    )
endif (ENABLE_TRACY)

if (INCLUDE_DEBUG_FEATURES)
    target_compile_definitions(imguiapp PUBLIC
        INCLUDE_DEBUG_FEATURES
    )
endif (INCLUDE_DEBUG_FEATURES)

target_include_directories(imguiapp PUBLIC
    ${CMAKE_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/bindings
    ${imgui_PACKAGE_FOLDER_${BUILDTYPE}}/res/misc/cpp
)
