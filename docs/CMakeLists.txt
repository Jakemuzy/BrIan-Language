cmake_minimum_required(VERSION 3.16)
project(BrIan C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER clang)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-c23-extensions -Wall -Wextra -Wpedantic -g")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

file(GLOB LIB_SOURCES "src/*.c")
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/main.c")
add_library(blibs STATIC ${LIB_SOURCES})
target_include_directories(blibs PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(compiler src/main.c)
target_link_libraries(compiler PRIVATE blibs)
set_target_properties(compiler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

add_subdirectory(tests)