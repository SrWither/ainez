cmake_minimum_required(VERSION 3.7)
project(EizNess)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SDL2 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include ${SDL2_INCLUDE_DIRS})
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_subdirectory(src)
add_subdirectory(test)
