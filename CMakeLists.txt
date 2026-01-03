cmake_minimum_required(VERSION 3.16)

project(SerialTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt6安装路径
set(Qt6_DIR "C:/Qt/6.10.1/mingw_64/lib/cmake/Qt6")

# 查找Qt6 Widgets和SerialPort模块
find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

qt_add_executable(SerialTool
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

# 链接Qt6 Widgets和SerialPort库
target_link_libraries(SerialTool PRIVATE Qt6::Widgets Qt6::SerialPort)

# 设置目标属性
set_target_properties(SerialTool PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SerialTool
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    # 设置应用图标
    WIN32_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/icon.ico"
)

# 安装配置
include(GNUInstallDirs)
install(TARGETS SerialTool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 最终化可执行文件
qt_finalize_executable(SerialTool)