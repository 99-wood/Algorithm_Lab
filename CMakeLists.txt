cmake_minimum_required(VERSION 3.31)
project(maze_game)

set(CMAKE_CXX_STANDARD 20)
include_directories("./include/include")
add_executable(maze_game main.cpp
        maze.cpp
        maze.h)
