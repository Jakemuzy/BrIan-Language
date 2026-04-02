cmake_minimum_required(VERSION 3.16)
project(BrIan C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER clang)

include_directories(${CMAKE_SOURCE_DIR}/include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

#add_subdirectory(tests)
file(GLOB LIB_SOURCES "src/*.c")
add_executable(compiler src/main.c ${LIB_SOURCES})