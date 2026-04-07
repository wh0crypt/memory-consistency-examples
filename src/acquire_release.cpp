/**
 * University of La Laguna
 * Higher School of Engineering and Technology
 * Project: Memory Consistency Examples
 * @file acquire_release.cpp
 * @author Domenico Goya (wh0crypt@computer.org)
 * @brief Demonstration of Acquire/Release memory semantics.
 * 
 * This program illustrates how to establish a "happens-before" 
 * relationship between two threads using atomic operations to 
 * prevent memory reordering in multicore architectures.
 * 
 * @version 1.0
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
 * @note The use of Acquire/Release semantics creates a formal 
 * synchronization barrier between the producer and the consumer.
 */
int shared_payload = 0;
std::atomic<int> sync_flag{0};

/**
 * @brief Producer thread logic using Release semantics.
 * @details Performs a store with 'memory_order_release'. This guarantees 
 * that all prior memory writes (like shared_payload) are visible to any 
 * thread that performs an 'acquire' load on this same atomic variable.
 * @note Generates a 'stlr' instruction on ARM64 or a 'dmb' on ARM32.
 */
void producer() {
// Simulate computational latency
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    shared_payload = 42; 

    // RELEASE STORE: Acts as a gateway. No previous memory operations 
    // can be reordered AFTER this store.
    sync_flag.store(1, std::memory_order_release);
}

/**
 * @brief Consumer thread logic using Acquire semantics.
 * @details Performs a load with 'memory_order_acquire'. This guarantees 
 * that no subsequent memory reads (like shared_payload) can be executed 
 * before this load is completed.
 * @note Generates a 'ldar' instruction on ARM64 or a compiler barrier on x86.
 */
void consumer() {
    // ACQUIRE LOAD: Acts as a gateway. No subsequent memory operations 
    // can be reordered BEFORE this load.
    while (sync_flag.load(std::memory_order_acquire) != 1) {
        // Optimization: prevent the loop from consuming 100% CPU cycles
        std::this_thread::yield();
    }

    // GUARANTEED: Due to the happens-before relationship, shared_payload 
    // will always be 42 here, regardless of CPU architecture.
    std::cout << "Acquire/Release Payload Result: " << shared_payload << std::endl;
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
