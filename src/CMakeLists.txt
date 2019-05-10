
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

# disallow make clean to delete ts files
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
SET(TS_FILES
        translations/shadowshuttle-deepin.en.ts
        translations/shadowshuttle-deepin.zh_CN.ts
        )
# create translation files
qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

# add Search path for AUTOUIC
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

#add_dependencies(../libQtShadowsocks)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UIS_HDRS} ${RESOURCES} ${QM_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/src/3rd_party/include/botan-2
${CMAKE_SOURCE_DIR}/src/3rd_party/include/QtShadowsocks)
target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES}
${CMAKE_SOURCE_DIR}/src/3rd_party/lib/libQtShadowsocks.a
${CMAKE_SOURCE_DIR}/src/3rd_party/lib/libbotan-2.a
)

qt5_use_modules(${PROJECT_NAME} ${QT})
set(CMAKE_INSTALL_PREFIX /usr)
# Install qm files
install(FILES ${QM_FILES}
        DESTINATION share/shadowshuttle-deepin/translations)

# Install icons
foreach (ICON_SIZE 16 24 32 48 64 96 128 256)
    install(FILES Resources/ssw${ICON_SIZE}.svg
            RENAME shadowshuttle-deepin.svg
            DESTINATION share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps)
endforeach ()
install(TARGETS shadowshuttle-deepin DESTINATION bin)
install(FILES ../shadowshuttle-deepin.desktop DESTINATION share/applications)
