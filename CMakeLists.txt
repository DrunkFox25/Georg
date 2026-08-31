cmake_minimum_required(VERSION 3.16)

project(Geogebra2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
)

FetchContent_MakeAvailable(Eigen)

add_executable(geo GeorgEngine.cpp)

target_link_libraries(geo PRIVATE Eigen3::Eigen)