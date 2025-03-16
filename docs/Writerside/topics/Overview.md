# Overview

This project by **Agnirudra Sil**, **Jaicharan Yeluri**, **Yash Kumar Singh**, is to build a communication system and a
navigation system for our college.

Name
: Agnirudra Sil

Roll No:
: 231CV104

E-mail
: agnirudra.sil@gmail.com

Phone
: +91 73494 46630

## Concept

There are two main aspects of this projects:

* **Communication System**: The aim of this project is to establish a single communication system for the entire
  college,
  allowing for streamlined, effective communication with all users, with minimal effort. This project is based on a
  Discord clone project built by Agnirudra Sil during high school. The other team members built upon this foundation to
  streamline development and bring the idea closer to realization.
* **Maps**: This project focused on creating a local navigation system tailored specifically for our college campus. The
  goal of this system is to help students, staff, and visitors find the quickest path to any building or location within
  the campus grounds. By calculating the shortest route, this system makes it easy for anyone to navigate efficiently
  from
  one point to another, saving time and reducing confusion.

## Code and Repository

The code for the projects is available on GitHub:

* **Communication System**: [](https://github.com/agnirudrasil/avault)
* **Maps**: [](https://github.com/agnirudrasil/geocom)
    * The communication system is also available as a GIT submodule in the Maps repository.

All the important files linked in this document are commented with the **Sphynx** or **Doxygen** style comments.

## Data Structures and Algorithms

### Data Structures

A list of data structures used in the projects:

Linked List
: Used to implement the queue

Queue
: Used to process multiple messages when several users are posting messages in the same group

Binary Heap
: Used to store the nodes and their distances from the source node in the shortest path algorithms

Adjacency List
: Used to represent the graph in the shortest path algorithms

Graph
: Used to represent the campus layout in the navigation system

### Algorithms

A list of algorithms used in the projects:

A* Algorithm
: An algorithm used to find the shortest path between two points on the campus. It uses heuristics to improve
performance

Dijkstra's Algorithm
: Another algorithm used to find the shortest path between two points on the campus. It is a special case of A* with a
heuristic of zero
