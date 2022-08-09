cmake_minimum_required(VERSION 3.5)

project(learn3-2 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

#find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets Multimedia OpenGL REQUIRED)
include_directories(/usr/include/x86_64-linux-gnu)

if(ANDROID)
  add_library(learn3-2 SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    XUnpackage.h
    XUnpackage.cpp
    XDecode.h
    XDecode.cpp
    XResample.h
    XResample.cpp
    XParMng.h
    XPlayDevice.h

    XAudioDevice.cpp
    XVideoWidget.h
    XVideoWidget.cpp
  )
else()
  add_executable(learn3-2
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    XUnpackage.h
    XUnpackage.cpp
    XDecode.h
    XDecode.cpp
    XResample.h
    XResample.cpp
    XParMng.h
    XPlayDevice.h

    XAudioDevice.cpp
    XVideoWidget.h
    XVideoWidget.cpp
  )
endif()

#target_link_libraries(learn3-2 PRIVATE Qt5::Widgets)

target_link_libraries(learn3-2 PRIVATE
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::OpenGL
    avcodec
    avformat
    avutil
    swresample
    swscale
    swscale
    avfilter
    )

