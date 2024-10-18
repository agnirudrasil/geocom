cmake_minimum_required(VERSION 3.30)
project(pathfinding)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(pathfinding main.cpp)

target_link_libraries(pathfinding PRIVATE nlohmann_json::nlohmann_json)
