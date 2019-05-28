set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT Core Gui Widgets Network DBus Sql LinguistTools)
find_package(Qt5 REQUIRED ${QT})

find_package(PkgConfig REQUIRED)
pkg_search_module(DTKWIDGET REQUIRED dtkwidget)
pkg_search_module(DFRAMEWORKDBUS REQUIRED dframeworkdbus)
pkg_search_module(DTKCORE REQUIRED dtkcore)
pkg_search_module(QTSHADOWSOCKS REQUIRED QtShadowsocks)
pkg_search_module(LIBQRENCODE REQUIRED libqrencode)
pkg_search_module(ZBAR REQUIRED zbar)

set(3RD_LIBS
    ${DTKCORE_LIBRARIES}
    ${DTKWIDGET_LIBRARIES}
    ${DFRAMEWORKDBUS_LIBRARIES}
    ${QTSHADOWSOCKS_LIBRARIES}
    ${LIBQRENCODE_LIBRARIES}
    ${ZBAR_LIBRARIES}
)

set(3RD_INCLUDES
    ${DTKCORE_INCLUDE_DIRS}
    ${DTKWIDGET_INCLUDE_DIRS}
    ${DFRAMEWORKDBUS_INCLUDE_DIRS}
    ${QTSHADOWSOCKS_INCLUDE_DIRS}
    ${LIBQRENCODE_INCLUDE_DIRS}
    ${ZBAR_INCLUDE_DIRS}
)

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

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")
# add Search path for AUTOUIC
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# disallow make clean to delete ts files
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
SET(TS_FILES
        translations/shadowshuttle-deepin.en.ts
        translations/shadowshuttle-deepin.zh_CN.ts
        )
# create translation files
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UIS_HDRS} ${RESOURCES} ${QM_FILES})
target_link_libraries(${PROJECT_NAME} ${3RD_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${3RD_INCLUDES})

qt5_use_modules(${PROJECT_NAME} ${QT})

# create debian package
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_DEBIAN_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-all.deb")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
set(CPACK_DEBIAN_PACKAGE_SECTION "web")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION ${PROJECT_DESCRIPTION})
set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtshadowsocks (>=2.0.1-1), libqrencode3 (>=3.4.4-1+b2), libzbar0 (>=0.10+doc-10.1+b2)")
set(CPACK_PACKAGE_CONTACT "joseph1002.lee@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "wallestar")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/joseph1002/shadowshuttle-deepin")

set(CMAKE_INSTALL_PREFIX /usr)
# Install qm files
install(FILES ${QM_FILES}
        DESTINATION share/shadowshuttle-deepin/translations)
# Install icons
foreach (ICON_SIZE 16 24 32 36 48 64 96 128 256)
    install(FILES resources/icons/${ICON_SIZE}x${ICON_SIZE}/ss.svg
            RENAME shadowshuttle-deepin.svg
            DESTINATION share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps)
endforeach ()
install(TARGETS shadowshuttle-deepin DESTINATION bin)
install(FILES ../shadowshuttle-deepin.desktop DESTINATION share/applications)

include(CPack)
