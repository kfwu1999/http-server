#-------------------------------------------------------------------------------
#  - Project
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)
project(http-server)

#-------------------------------------------------------------------------------
#  - Configuration
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clangd

#-------------------------------------------------------------------------------
#  - lib
#-------------------------------------------------------------------------------
include_directories(include)
file(GLOB SOURCES src/*.cpp)
add_executable(main ${SOURCES} main.cpp)

#-------------------------------------------------------------------------------
#  - Subdirectories
#-------------------------------------------------------------------------------
add_subdirectory(test)
