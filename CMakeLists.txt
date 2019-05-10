# cmake minimum version
cmake_minimum_required(VERSION 3.11.2)
project(shadowshuttle-deepin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(PkgConfig REQUIRED)
set(QT Core Gui Widgets Network DBus Sql LinguistTools)
find_package(Qt5 REQUIRED ${QT})

pkg_check_modules(3rd_lib REQUIRED
#        QtShadowsocks
        dtkwidget dframeworkdbus
        dtkcore
        libqrencode zbar
        )

#if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
#    set(CMAKE_CXX_FLAGS -O3)
#    # generate qm files
#    execute_process(COMMAND bash "translate_generation.sh"
#                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
##else ()
##    set(CMAKE_CXX_COMPILER "clang++")
#endif ()


#============================================================================
# 3rd-party libraries
#============================================================================

# install conan 3rdparty libraries to package dirs.
#message("\nStart generate 3rdparty dependencies as ${CMAKE_BUILD_TYPE}:\n")
#if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
#        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
#        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
#                                        "${CMAKE_BINARY_DIR}/conan.cmake")
#endif()
#include(${CMAKE_BINARY_DIR}/conan.cmake)
#conan_check(REQUIRED)
#conan_cmake_run(
#        CONANFILE conanfile.txt
#        INSTALL_FOLDER ${CMAKE_BINARY_DIR}
#        BUILD_TYPE ${CMAKE_BUILD_TYPE}
#        BUILD missing
#)
#include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

## setup 3rd-party package
#find_package(quazip REQUIRED)
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()


# 添加子目录 src
add_subdirectory(src)
