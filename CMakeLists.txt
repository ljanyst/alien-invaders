
cmake_minimum_required(VERSION 3.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/TM4C_toolchain.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
include(Firmware)

project(silly-invaders)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(drivers)
add_subdirectory(tm4c)
add_subdirectory(io)
add_subdirectory(tests)
add_subdirectory(game)
