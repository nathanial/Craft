cmake_minimum_required(VERSION 2.8)

project(craft)

add_definitions(-std=c++17 -fpermissive)
add_subdirectory(src)
add_subdirectory(deps/glfw)
