cmake_minimum_required(VERSION 3.16)

project(SubSurfaceApp VERSION 0.1 LANGUAGES CXX)
enable_language(CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Quick
    QuickControls2
    Graphs
    Sql
    Concurrent
)

# 1. Define where the library lives.
# We default to your path, but allow it to be overridden via command line
set(LIBDC_ROOT "${CMAKE_SOURCE_DIR}/lib/android_libs/libdivecomputer-0.9.0/dist/android_arm64" CACHE PATH "Path to Android libdc")

if(ANDROID)
    # When building for Android, we force CMake to look ONLY in your cross-compiled folder
    find_path(LIBDC_INCLUDE_DIR
        NAMES libdivecomputer/version.h
        HINTS ${LIBDC_ROOT}
        PATH_SUFFIXES include
        NO_DEFAULT_PATH
        CMAKE_FIND_ROOT_PATH_BOTH  # <--- CRITICAL ADDITION
    )

    find_library(LIBDC
        NAMES divecomputer
        HINTS ${LIBDC_ROOT}
        PATH_SUFFIXES lib
        NO_DEFAULT_PATH
        CMAKE_FIND_ROOT_PATH_BOTH  # <--- CRITICAL ADDITION
    )

    # IMPORTANT: Force CMake to update if it found them
    if(LIBDC_INCLUDE_DIR AND LIBDC)
        message(STATUS "SUCCESS: Found LIBDC_INCLUDE_DIR: ${LIBDC_INCLUDE_DIR}")
        message(STATUS "SUCCESS: Found LIBDC: ${LIBDC}")
    else()
        message(WARNING "FAILURE: CMake still cannot see the files in ${LIBDC_ROOT}")
    endif()
else()
    # Keep your existing logic for Mac/Linux development
    find_path(LIBDC_INCLUDE_DIR
        NAMES libdivecomputer/version.h
        HINTS ENV HOMEBREW_PREFIX /opt/homebrew /usr/local
        PATH_SUFFIXES include
        REQUIRED
    )
    find_library(LIBDC
        NAMES divecomputer
        HINTS ENV HOMEBREW_PREFIX /opt/homebrew /usr/local
        PATH_SUFFIXES lib
        REQUIRED
    )
endif()

qt_standard_project_setup(REQUIRES 6.8)


set(CMAKE_AUTORCC ON)

qt_add_executable(appSubSurfaceApp
    src/main.cpp
)

qt_add_qml_module(appSubSurfaceApp
    URI SubSurfaceApp
    QML_FILES
        qml/Main.qml
        qml/components/DiveGraphView.qml
        qml/pages/DiveList.qml
        qml/pages/DiveDetails.qml
        qml/pages/DiveGraphPage.qml
        qml/pages/AddDives.qml
        qml/pages/SettingsPage.qml
        qml/pages/DevicePickerDialog.qml
        qml/overlay/AddDiveButton.qml
        qml/overlay/Header.qml
    RESOURCES
        resources/resources.qrc
    SOURCES
        src/divecomputerwrapper.h
        src/divecomputerwrapper.cpp
        src/divedatabase.h
        src/divedatabase.cpp
        src/DiveDataStructure.h
        src/divelistmodel.h
        src/divelistmodel.cpp
        src/samplemodel.h
        src/samplemodel.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appSubSurfaceApp PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appSubSurfaceApp
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appSubSurfaceApp PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Graphs
    Qt6::Sql
    Qt6::Concurrent
    ${LIBDC}
)

if(ANDROID)
    # This tells Qt's deployment tool to include the library in the APK
    set_property(TARGET appSubSurfaceApp APPEND PROPERTY
        QT_ANDROID_EXTRA_LIBS ${LIBDC}
    )
endif()


target_include_directories(appSubSurfaceApp PRIVATE ${LIBDC_INCLUDE_DIR})


include(GNUInstallDirs)
install(TARGETS appSubSurfaceApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
