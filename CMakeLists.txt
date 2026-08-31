cmake_minimum_required(VERSION 3.16)

project(Geogebra2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

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
