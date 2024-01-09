cmake_minimum_required(VERSION 3.18)
project(LAClient C)

set(CMAKE_C_STANDARD 11)

add_executable(LAClient main.c)
target_link_libraries(LAClient pthread)
