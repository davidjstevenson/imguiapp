
if (IMGUIAPP_VERSION_ZERO)

    set(VERSION_MAJOR 0)
    set(VERSION_MINOR 0)
    set(VERSION_TYPE zero)

    string(TIMESTAMP VERSION_BUILD_DATE "%Y%m%d")
    set(GIT_HASH "000000000")

else()

    execute_process(COMMAND ${GIT_EXECUTABLE} git rev-parse --short HEAD OUTPUT_VARIABLE GIT_HASH)
    execute_process(COMMAND ${GIT_EXECUTABLE} git log -1 --format=%cd --date=format:%Y/%m/%d OUTPUT_VARIABLE VERSION_BUILD_DATE)
    string(STRIP ${GIT_HASH} GIT_HASH)
    string(STRIP ${VERSION_BUILD_DATE} VERSION_BUILD_DATE)

    set(VERSION_MAJOR 0)
    set(VERSION_MINOR 1)
    set(VERSION_TYPE alpha)

endif()

# The variable will be used when file is configured
configure_file(${INPUT_FILE}/version.h.in ${OUTPUT_FILE}/version.h)
message("GIT_VERSION: ${VERSION_MAJOR}.${VERSION_MINOR}-${GIT_HASH}")
