cmake_minimum_required(VERSION 3.28.2)
project(pathfinding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(pathfinding main.cpp
        heap.h
        graph.h
        node.h
        pathfinding.tpp
        pathfinding.h
        graph.tpp
        edge.h
        heap.tpp
        utils.h
        utils.cpp
        bind.cpp
        bind.h
        node_visitor.h
)

target_link_options(pathfinding PRIVATE --emit-tsd pathfinding.d.ts --bind -sMODULARIZE=1 -sENVIRONMENT=node)
target_link_libraries(pathfinding PRIVATE nlohmann_json::nlohmann_json)
