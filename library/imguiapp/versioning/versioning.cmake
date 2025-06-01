
set(VERSIONING_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/versioning/version.h.in
)

execute_process(
    COMMAND ${CMAKE_COMMAND}
    -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
    -D INPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/versioning/
    -D OUTPUT_FILE=${CMAKE_BINARY_DIR}/versioning/
    -D IMGUIAPP_VERSION_ZERO=${IMGUIAPP_VERSION_ZERO}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/versioning/generate_version.cmake
)

add_custom_target(GitVersion
    COMMAND ${CMAKE_COMMAND}
    -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
    -D INPUT_FILE=${CMAKE_SOURCE_DIR}/imguiapp/versioning/
    -D OUTPUT_FILE=${CMAKE_BINARY_DIR}/versioning/
    -D IMGUIAPP_VERSION_ZERO=${IMGUIAPP_VERSION_ZERO}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/imguiapp/versioning/generate_version.cmake
)
