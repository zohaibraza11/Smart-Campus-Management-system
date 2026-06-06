# Smart Campus Management System

A comprehensive, console-based Smart Campus Management System designed to optimize university campus navigation, logistics, and data lookup. This project is built entirely from scratch in C++ without using standard container libraries (STL), showcasing custom implementations of core Data Structures and Algorithms (DSA).

##  Key Features

- **Module 1: Intelligent Navigation (Graph + Dijkstra)**
  - Graph-based campus mapping with custom adjacency matrix representation.
  - Shortest path finding using **Dijkstra's Algorithm**.
  - Path exploration using Breadth-First Search (**BFS**) and Depth-First Search (**DFS**).
- **Module 2: Location Directory Management (Binary Search Tree)**
  - Hierarchical storage of campus locations using a balanced approach.
  - Supports dynamic insertions, deletions, and $O(\log n)$ lookup times.
- **Module 3: Campus Request Handling (Queue)**
  - First-In, First-Out (**FIFO**) queue utilizing a custom linked list to handle campus helpdesk or administrative requests sequentially.
- **Module 4: Action History Tracking (Stack)**
  - Last-In, First-Out (**LIFO**) stack to track user activities and handle multi-level "Undo" actions.
- **Module 5: Fast Infrastructure Lookup (Hash Table)**
  - **Open Addressing with Linear Probing** hash table for instantaneous $O(1)$ lookup of infrastructure coordinates/IDs.
- **Module 6: Sorting & Searching Engine**
  - Custom **Bubble Sort** implementation to sort locations alphabetically or dynamically by distance from the Main Gate.
  - **Binary Search** implementation for rapid name lookups on sorted datasets.

---

## Data Structures Implemented From Scratch

To demonstrate low-level memory management and algorithm architecture, this project strictly avoids STL containers and utilizes custom-built:
* **`DynArray<T>`** (Dynamic Array/Vector template with automatic resizing)
* **Adjacency Matrix Graph** (Weighted, undirected)
* **Binary Search Tree (BST)**
* **Linked-List based Queue & Stack**
* **Hash Table with Linear Probing**

---

##  How to Run Locally

### Prerequisites
Make sure you have a C++ compiler installed (e.g., `g++` via MinGW/GCC) and configured in your environment path.

### Steps to Execute
1. **Clone the repository:**
   ```bash
   git clone [https://github.com/YOUR_USERNAME/smart-campus-management-system.git](https://github.com/YOUR_USERNAME/smart-campus-management-system.git)
   cd smart-campus-management-system
