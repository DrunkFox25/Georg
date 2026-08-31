# Install script for directory: /workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/AdolcForward"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/AlignedVector3"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/ArpackSupport"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/AutoDiff"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/BVH"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/EulerAngles"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/FFT"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/IterativeSolvers"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/KroneckerProduct"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/LevenbergMarquardt"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/MatrixFunctions"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/MoreVectorization"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/MPRealSupport"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/NonLinearOptimization"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/NumericalDiff"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/OpenGLSupport"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/Polynomials"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/Skyline"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/SparseExtra"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/SpecialFunctions"
    "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/Splines"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/workspaces/Georg/build/_deps/eigen-src/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/workspaces/Georg/build/_deps/eigen-build/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

