#
# directory structure: . ├── build ├── CMakeLists.txt ├── include |   └── *.h
# └── source └── *.cpp
#
# ##############################################################################
# usage: execute "cmake .." from build directory
# ##############################################################################

cmake_minimum_required(VERSION 3.13)

#
# project name (=identical to executable name)
#
project(qt-coordsys)

#
# Compiler flags
#
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pedantic")

#
# configure qt
#
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#
# add include directories for header files)
#
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(
  /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include
)

# Can manually add the sources using the set command as follows: set(SOURCES
# hello.cpp)

# However, the file(GLOB...) allows for wildcard additions:
set(SOURCES src/main.cpp src/coordsys.cpp src/w_coordsys_view.cpp
            src/coordsys_model.cpp)
set(HEADERS include/coordsys.hpp include/w_coordsys_view.hpp
            include/coordsys_model.hpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

#
# fmt library
#
find_package(fmt)
target_link_libraries(${PROJECT_NAME} fmt::fmt)

#
# Use the Widgets module from Qt 6.
#
# Find the QtWidgets library
find_package(
  Qt6
  COMPONENTS Widgets
  REQUIRED)
target_link_libraries(${PROJECT_NAME} Qt6::Widgets)
