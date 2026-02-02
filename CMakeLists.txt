cmake_minimum_required(VERSION 3.16)

project(MiniDBMS_GUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(GUI_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

set(CORE_SOURCES
    core/Database.cpp
    core/Parser.cpp
    core/Table.cpp
    core/Utils.cpp
)

set(CORE_HEADERS
    core/Database.h
    core/Parser.h
    core/Table.h
    core/Utils.h
    core/Column.h
    core/Row.h
)

qt_add_executable(MiniDBMS_GUI
    ${GUI_SOURCES}
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

target_include_directories(MiniDBMS_GUI PRIVATE core)

target_link_libraries(MiniDBMS_GUI PRIVATE Qt6::Widgets)
