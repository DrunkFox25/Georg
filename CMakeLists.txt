cmake_minimum_required(VERSION 3.16)

project(Geogebra2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(DEFINED ENV{CODESPACES})
    set(QT_SEARCH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/qt/6.7.2/gcc_64")
    if(EXISTS "${QT_SEARCH_PATH}")
        list(APPEND CMAKE_PREFIX_PATH "${QT_SEARCH_PATH}")
        message(STATUS "Auto-detected valid Qt version at: ${QT_SEARCH_PATH}")
    endif()
endif()

find_package(Qt6 6.7 REQUIRED COMPONENTS Widgets Core Gui)# 6767676767

include(FetchContent)
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
)
FetchContent_MakeAvailable(Eigen)

FetchContent_Declare(
    JKQTPlotter6
    GIT_REPOSITORY https://github.com/jkriege2/JKQtPlotter.git
    GIT_TAG        v5.0.0
)

FetchContent_MakeAvailable(JKQTPlotter6)

add_executable(geo src/main.cpp src/GeorgEngine.cpp src/reader.cpp src/util.cpp src/GeorgInterface.cpp src/expression.cpp src/util.cpp)

target_link_libraries(geo PRIVATE 
    Eigen3::Eigen
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    JKQTPlotter6::JKQTPlotter6
)







