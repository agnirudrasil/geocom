# Maps

Contributor: **Agnirudra Sil** and **Yash Kumar Singh**

## Introduction

This project focused on creating a local navigation system tailored specifically for our college campus. The goal of
this system is to help students, staff, and visitors find the quickest path to any building or location within the
campus grounds. By calculating the shortest route, this system makes it easy for anyone to navigate efficiently from one
point to another, saving time and reducing confusion.

This system is especially useful for new students, such as first-year students, who might not be familiar with the
layout of the campus yet.
Instead of struggling to find classrooms, labs, or other facilities, users can rely on this
navigation tool to confidently get where they need to go.
Ultimately, this project aims to make our campus more
accessible and welcoming, ensuring that everyone—especially newcomers—can move around smoothly and make the most of
their time here.

## Design

The various data structures and algorithms are implemented in the C++ programming language.
The frontend is an iOS app.
To allow interaction between the app and the algorithms, a REST API written in Typescript is used.
The Typescript API accesses C++ classes and functions through the WebAssembly module.
The C++ code is compiled to WebAssembly using Emscripten.
The iOS app communicates with the API to get the shortest path between two locations on the campus.

## Implementation

Two shortest path algorithms have been implemented for this project. The first is the A* algorithm, which is an
algorithm that uses heuristics to improve performance. The second is Dijkstra's algorithm, which is a special case of A*
with a heuristic of zero.

<br />

* **Header
  File**: [](https://github.com/agnirudrasil/geocom/blob/main/pathfinding/pathfinding.h)
* **Implementation File**: [](https://github.com/agnirudrasil/geocom/blob/main/pathfinding/pathfinding.tpp)

Both the algorithms use a binary heap data structure to store the nodes and their distances from the source node. The
binary heap implementation is based on the `heapq` module in Python.

* **Header File**: [](https://github.com/agnirudrasil/geocom/blob/main/pathfinding/heap.h)
* **Implementation File**: [](https://github.com/agnirudrasil/geocom/blob/main/pathfinding/heap.tpp)

The graph is represented as an adjacency list, where each node has a list of its neighbors.

* **Header File**: [](https://github.com/agnirudrasil/geocom/blob/main/pathfinding/graph.h)
* **Implementation File**: [](https://github.com/agnirudrasil/geocom/blob/main/pathfinding/graph.tpp)

## Execution

Let us take a realistic situation, Suppose I am a first year student.
Being new to the college, I am not very much aware
of the campus.
Let us say there is an event taking place at LHC-C and I wish to know the quickest path to reach there.
Here is where
this system would come to aid.

First, Open the app and enter your current location (In my case, block 7 hostel- Sahyadri) then enter the location you
wish to reach-destination (LHC-C).

![directions](directions.png)

The app would immediately give me the shortest path to reach LHC-C.

![shortest_path.png](shortest_path.png)
