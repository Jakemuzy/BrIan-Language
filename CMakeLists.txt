cmake_minimum_required(VERSION 3.16)

project(BrIan C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER clang)

# llvm crap

include_directories(${CMAKE_SOURCE_DIR}/includes)
include_directories(${CMAKE_SOURCE_DIR}/includes/DataStructures)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g")# -Wextra -Wpedantic -g")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/builds)

file(GLOB LIB_SOURCES "src/*.c")
add_subdirectory(TestCases)
