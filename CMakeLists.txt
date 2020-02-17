cmake_minimum_required(VERSION 3.5)
project(cs210_simple_group_30 C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(cs210_simple_group_30
        src/main.c
        src/simpleShell.c
        src/simpleShell.h)
