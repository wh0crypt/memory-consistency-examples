/**
 * University of La Laguna
 * Higher School of Engineering and Technology
 * Project: Memory Consistency Examples
 * @file volatile.c
 * @author Domenico Goya (wh0crypt@computer.org)
 * @brief Demonstration of the 'volatile' qualifier in C and its implications for memory consistency.
 * 
 * This example shows how the 'volatile' qualifier prevents certain compiler optimizations, ensuring that
 * reads and writes to the variable 'x' are not optimized away. However, it does not provide any guarantees
 * about atomicity or ordering, and thus is not sufficient for safe concurrent access in multithreaded programs.
 * 
 * @version 0.1
 * @date 2026-05-11
 * @copyright Copyright (c) 2026 Domenico Goya (wh0crypt)
 */


int main() {
    // The 'volatile' qualifier tells the compiler that the value of 'x' may change at any time
    // without any action being taken by the code the compiler finds nearby.
    volatile int x = 10;
    x = 42;
    return x;
}
