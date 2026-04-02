cmake_minimum_required(VERSION 3.21)
project(SmartPunch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Charts)
qt_standard_project_setup()

qt_add_executable(SmartPunch
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    Info.plist
)

target_link_libraries(SmartPunch PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Charts
)

if(APPLE)
    set_target_properties(SmartPunch PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
        QT_IOS_DEVICE_FAMILY 1
        QT_IOS_DEPLOYMENT_TARGET 15.0
    )
endif()