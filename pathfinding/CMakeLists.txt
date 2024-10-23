cmake_minimum_required(VERSION 3.30)
project(pathfinding)

set(CMAKE_CXX_STANDARD 20)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(pathfinding main.cpp
        heap.h
        graph.h
        node.h
        astar.cpp
        astar.h
        graph.tpp
        edge.h
        heap.tpp
)

target_link_libraries(pathfinding PRIVATE nlohmann_json::nlohmann_json)
