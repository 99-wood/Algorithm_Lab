cmake_minimum_required(VERSION 3.31)
project(maze_game)

set(CMAKE_CXX_STANDARD 20)
MATH(EXPR stack_size "100*1024*1024")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")
include_directories("./include/include")
add_executable(maze_game main.cpp
        maze.cpp
        maze.h
        dp.cpp
        dp.h)

add_executable(sha256test sha256_test.c
        sha256.h
        sha256.c
        )
