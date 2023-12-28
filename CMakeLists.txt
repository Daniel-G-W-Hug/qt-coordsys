#[[
# directory structure:
# .
# ├── build
# ├── CMakeLists.txt
# ├── include
# |   └── *.[h|hpp]
# └── source
#     └── *.[cpp|cxx]
#
# ##############################################################################
# to configure execute from build directory:
#                cmake ..
#             or cmake .. -G "Ninja"
#             or cmake .. -G "Visual Studio 17 2022" -T host=x64 -A x64
#
# to build execute from build directory:
#
#                cmake --build .
#             or ninja
#
# ##############################################################################
#]]

cmake_minimum_required(VERSION 3.25)

# to use gcc-13 and g++-13 (must come before project line)
#[[
# set(CMAKE_C_COMPILER "gcc-13")
# set(CMAKE_CXX_COMPILER "g++-13")
#]]


project(
  qt-coordsys
  VERSION 0.1
  LANGUAGES CXX)

# set(CMAKE_VERBOSE_MAKEFILE ON)  // alternative: make VERBOSE=1
# add_definitions(-Dxxx)

#
# Compiler flags
#
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# configure qt
#
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# set a default build type: Debug | RelWithDebInfo | Release | MinSizeRel
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  message(STATUS "Build type not specified: use Debug mode by default.")
endif()

if(CMAKE_BUILD_TYPE EQUAL "Debug")
  if(MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
  else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
  endif()
endif()

# However, the file(GLOB...) allows for wildcard additions:
set(SOURCES src/main.cpp src/coordsys.cpp src/w_coordsys.cpp src/w_cs_view.cpp
            src/coordsys_model.cpp src/w_statusbar.cpp)
set(HEADERS include/coordsys.hpp include/w_coordsys.hpp include/w_cs_view.hpp
            include/coordsys_model.hpp include/w_statusbar.hpp)

set(EXEC_NAME ${PROJECT_NAME})
add_executable(${EXEC_NAME} ${SOURCES} ${HEADERS})

# target link libraries have to be added AFTER add_executable or add_library!
target_include_directories(${EXEC_NAME} PRIVATE include)
target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${EXEC_NAME}
                           PRIVATE ${CMAKE_SOURCE_DIR}/../../include)

#
# Use the Widgets module from Qt 6.
#
# Find the QtWidgets library
find_package(
  Qt6
  COMPONENTS Core Gui Widgets 
  REQUIRED)

target_link_libraries(${EXEC_NAME} PRIVATE Qt6::Core
                                           Qt6::Gui
                                           Qt6::Widgets
                                           )

# make fmt available
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${EXEC_NAME} PRIVATE fmt::fmt-header-only)