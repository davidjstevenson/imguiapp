cmake_minimum_required(VERSION 3.21.3)
project(imguiappexample)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
)

if (MSVC AND NOT windows_use_console)
    set(EXE_ARGS WIN32)
endif()

add_subdirectory(library)

add_executable(${PROJECT_NAME} ${EXE_ARGS} example/main.cpp)
target_link_libraries(${PROJECT_NAME} imguiapp)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    "/permissive-"
    /W3
)

if (MSVC_JMC) 
    target_compile_options(${PROJECT_NAME}
        PRIVATE
        "$<$<CONFIG:DEBUG>:/JMC>"
    )
endif(MSVC_JMC)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC

    WIN32_LEAN_AND_MEAN
    NOMINMAX
    USE_MATH_DEFINES
    _WIN32_WINNT=0x0601

    APPLICATION_NAME_STR="${PROJECT_NAME}"
)
