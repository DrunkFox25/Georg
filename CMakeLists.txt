cmake_minimum_required(VERSION 3.16)

project(Geogebra2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(DEFINED ENV{CODESPACES})
    file(GLOB QT_SEARCH_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Qt/6.[789].*/gcc_64")
    if(QT_SEARCH_PATH)
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

add_executable(geo main.cpp GeorgEngine.cpp)

target_link_libraries(geo PRIVATE 
    Eigen3::Eigen
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)







