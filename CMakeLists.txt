cmake_minimum_required(VERSION 3.22)
project(font2c C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(contrib)
add_subdirectory(src)
