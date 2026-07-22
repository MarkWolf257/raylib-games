cmake_minimum_required(VERSION 3.14)
project(RaylibGames)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
)

FetchContent_MakeAvailable(raylib)

link_libraries(raylib)
if (APPLE)
    link_libraries("-framework IOKit")
    link_libraries("-framework Cocoa")
    link_libraries("-framework OpenGL")
elseif (WIN32)
    link_libraries("-lgdi32")
    link_libraries("-lwinmm")
elseif (UNIX)
    link_libraries("-lX11")
    link_libraries("-lXrandr")
endif()

add_executable(snake src/snake.c)