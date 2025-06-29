cmake_minimum_required(VERSION 3.11)
project(maze_game)

set(CMAKE_CXX_STANDARD 20)
MATH(EXPR stack_size "100*1024*1024")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--stack,${stack_size}")
#include_directories("./include/include")
include_directories("./include")
add_executable(maze_game main.cpp
        maze.cpp
        maze.h
        dp.cpp
        dp.h
        sha256.h
        sha256.c
        puzzle_solver.h
        puzzle_solver.cpp
        boss_strategy.h
        boss_strategy.cpp
        maze_navigator.cpp)

add_executable(sha256test sha256_test.c
        sha256.h
        sha256.c
        )
add_executable(play play.cpp
        maze.cpp
        maze.h
        maze_navigator.cpp
        maze_navigator.h)

add_executable(game3d game3d.cpp
        glad.c
        shader.cpp
        shader.h
        stb_image.cpp
        stb_image.h
        texture.cpp
        texture.h
        cube.cpp
        cube.h
        object.cpp
        object.h
        Character.cpp
        character.h
        slime.cpp
        slime.h
        chest.cpp
        chest.h
        maze.cpp
        maze.h
        dp.cpp
        dp.h)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(game3d PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/libglfw3dll.a
        opengl32
)
add_custom_command(TARGET game3d POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/lib/glfw3.dll
        $<TARGET_FILE_DIR:game3d>
)