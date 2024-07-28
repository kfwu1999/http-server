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

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "No CMAKE_BUILD_TYPE selected, defaulting to ${CMAKE_BUILD_TYPE}")
endif()

#-------------------------------------------------------------------------------
#  - lib
#-------------------------------------------------------------------------------
#
include_directories(include)
file(GLOB SOURCES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

# spdlog
find_package(spdlog REQUIRED)

#-------------------------------------------------------------------------------
#  - Linker
#-------------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
