# Memory Consistency Examples

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)

A educational project for **Computer Architecture** at the **University of La Laguna (ULL)**. This repository demonstrates the critical impact of memory consistency models, compiler reordering, and CPU out-of-order execution on multithreaded synchronization.

## Overview

Modern CPUs and compilers often reorder memory operations to optimize performance and hide latency. While logically sound in single-threaded contexts, this behavior can break classic synchronization algorithms like **Peterson's Algorithm** in multicore systems.

This project provides two comparative examples:

1. **Relaxed Ordering:** Shows how `memory_order_relaxed` allows reordering, leading to potential race conditions on weakly-ordered architectures (e.g., ARM).
2. **Acquire/Release Semantics:** Demonstrates the use of memory fences to establish a formal "happens-before" relationship, ensuring data integrity across threads.

---

## Project Structure

* `src/relaxed_ordering.cpp`: Implementation using relaxed atomics (unsafe).
* `src/acquire_release.cpp`: Implementation using Acquire/Release barriers (safe).
* `CMakeLists.txt`: Build configuration generating independent binaries for each example.

## Building the Project

### Prerequisites

* **CMake** (version 3.16 or higher)
* **C++23 Compatible Compiler** (GCC 13+, Clang 15+)
* **Address Sanitizer (ASan)**: Highly recommended for detecting memory issues.

### Compilation

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

---

## Architectural Analysis

To truly understand the hardware interaction, examine the generated assembly code using a tool like [Compiler Explorer (Godbolt)](https://godbolt.org/). For convenience, the examples can be found [here](https://godbolt.org/z/jzsPEjKhx).

### Key Observations

* **On x86-64/amd64**: Due to the **Total Store Order (TSO)** model, you might see identical assembly for both versions (using `MOV`), as x86 provides strong hardware guarantees.
* **On ARM64/AArch64**: Look for the difference between a standard `STR` (Store) and the `STLR` (Store-Release Register) instruction, or the presence of a `DMB` (Data Memory Barrier), though the last one is more common in the 32 bits architecture.

## Technical Features

* **Doxygen Documentation**: All source files include standard-compliant headers and technical notes.
* **I/O Optimizations**: Uses `sync_with_stdio(false)` to minimize *Observer Effect*.
* **Modern C++ Practices**: Utilizes C++23 standards, brace initialization, and `std::this_thread::yield()` for efficient spin-locks.

---

## License

This project is licensed under the **MIT License**. See the [LICENSE](./LICENSE) file for details.

---

Copyright (c) 2026 Domenico Goya (wh0crypt)
