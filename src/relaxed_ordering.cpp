/**
 * University of La Laguna
 * Higher School of Engineering and Technology
 * Project: Memory Consistency Examples
 * @file relaxed_ordering.cpp
 * @author Domenico Goya (wh0crypt@computer.org)
 * @brief Demonstration of memory reordering risks using relaxed atomics.
 * 
 * This example shows how 'memory_order_relaxed' allows the compiler and the 
 * CPU to reorder stores and loads, potentially leading to race conditions 
 * in multicore systems.
 * 
 * @version 0.1
 * @date 2026-04-07
 * @copyright Copyright (c) 2026 Domenico Goya (wh0crypt)
 */

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

/**
 * @brief Global shared data.
 * @note In a 'relaxed' model, there is no "happens-before" relationship
 * guaranteed between shared_payload and sync_flag.
 */
int shared_payload = 0;
std::atomic<int> sync_flag{0};

/**
 * @brief Producer thread logic.
 * @details Writes to a data variable and then sets a flag.
 * Without proper fences, the write to 'sync_flag' could be observed by other
 * cores BEFORE the write to 'shared_payload' due to CPU store-buffer reordering.
 */
void producer() {
    // Simulate computational latency
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    shared_payload = 42;

    // RELAXED STORE: Only guarantees atomicity of the operation itself.
    // It does NOT act as a memory barrier for non-atomic variables.
    sync_flag.store(1, std::memory_order_relaxed);
}

/**
 * @brief Consumer thread logic.
 * @details Polls the flag and then reads the shared data.
 * Due to 'relaxed' loading, the CPU might speculatively read 'shared_payload'
 * before the 'sync_flag' check is actually completed.
 */
void consumer() {
    // Spin-lock pattern
    while (sync_flag.load(std::memory_order_relaxed) != 1) {
        // Optimization: prevent the loop from consuming 100% CPU on some archs
        std::this_thread::yield(); 
    }

    // ARCHITECTURE RISK: On weakly-ordered CPUs (ARM, POWER), this might 
    // print '0' if the read of shared_payload was reordered before the flag check.
    std::cout << "Relaxed Payload Result: " << shared_payload << std::endl;
}

int main() {
    // Speed up standard I/O and disable synchronization with C stdio
    // This minimizes the "Observer Effect" during thread execution.
    std::ios_base::sync_with_stdio(false);

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    producer_thread.join();
    consumer_thread.join();

    return EXIT_SUCCESS;
}
