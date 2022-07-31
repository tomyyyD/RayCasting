cmake_minimum_required(VERSION 3.22)
project(RayCasting)

set(CMAKE_CXX_STANDARD 14)

add_executable(RayCasting main.cpp)

include_directories(/opt/homebrew/include)

find_package(glfw3 REQUIRED)
target_link_libraries(RayCasting glfw)

find_package(GLEW REQUIRED)
target_link_libraries(RayCasting GLEW::GLEW)