/**
 * University of La Laguna
 * Higher School of Engineering and Technology
 * Project: Memory Consistency Examples
 * @file test_n_set.c
 * @author Domenico Goya (wh0crypt@computer.org)
 * @brief Demonstration of Test-and-Set (TAS) primitives and memory fencing.
 *
 * This program explores the implementation of synchronization primitives at
 * three levels of abstraction: high-level C logic, C11 standard atomics,
 * and x86 inline assembly. It serves as an educational tool to understand
 * how hardware-level atomicity and compiler barriers prevent race conditions
 * and maintain memory consistency in multiprocessor systems.
 *
 * @version 1.0
 * @date 2026-04-09
 * @copyright Copyright (c) 2026 Domenico Goya (wh0crypt)
 */

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef bool (*lock_func_t)(volatile uint8_t *);

/**
 * @brief Atomic Test-and-Set using the C11 Memory Model.
 *
 * This function performs an atomic exchange operation using the highest
 * level of memory consistency (Sequential Consistency). It ensures that
 * the swap is seen in the same order by all threads across all CPU cores.
 *
 * @param target Pointer to the volatile variable used as a lock.
 * @return true If the previous value was non-zero (lock was already held).
 * @return false If the previous value was zero (lock successfully acquired).
 */
static inline bool test_n_set_stdatomic(volatile uint8_t *target) {
    return atomic_exchange_explicit(
        (_Atomic uint8_t *)target,
        1,
        memory_order_seq_cst
    );
}

/**
 * @brief Non-atomic Test-and-Set implementation in pure C.
 *
 * @warning: This implementation is NOT thread-safe. It is provided for
 * educational purposes to demonstrate the race condition that occurs
 * without hardware-level atomicity.
 *
 * @param target Pointer to the lock variable.
 * @return true if the lock was already set, false otherwise.
 */
static inline bool test_n_set_c(volatile uint8_t *target) {
    volatile uint8_t previous = *target;
    *target = 1;
    return previous != 0;
}

/**
 * @brief Atomic Test-and-Set implementation using x86 inline assembly.
 *
 * This function uses the 'xchg' instruction, which is implicitly atomic
 * on x86 architectures when one operand is a memory location. 'xchg' with a
 * memory operand triggers an implicit LOCK prefix, asserting the
 * LOCK# signal on the bus to ensure atomicity across cores.
 *
 * @param target Pointer to the lock variable.
 * @return true If the lock was already set.
 * @return false If the lock was successfully acquired.
 */
static inline bool test_n_set_asm(volatile uint8_t *target) {
    uint8_t state = 1;
    __asm__ volatile("xchg [%1], %0" : "+r"(state) : "r"(target) : "memory");
    return (state != 0);
}

/**
 * @brief Simulates a critical section entry logic.
 *
 * This function demonstrates how memory operations (v += 3, etc.)
 * interact with synchronization primitives. The use of a function
 * pointer prevents the compiler from inlining and reordering
 * code across the "lock" boundary.
 *
 * @param x Pointer to the shared integer value.
 * @param lock Pointer to the synchronization primitive.
 * @param func Function pointer to the atomic test-and-set implementation.
 * @return true if the critical section was successfully entered and memory
 * updated.
 * @return false if the lock was already held by another entity.
 */
bool example(int *x, volatile uint8_t *lock, lock_func_t lock_func) {
    bool already_locked = (*lock_func)(lock);
    if (already_locked) {
        return false;
    }

    int value = *x;
    value += 3;
    value *= 2;
    --value;
    value += 10;

    *x = value;
    return true;
}

int main() {
    int data_c = 42;
    int data_asm = 42;
    int data_std = 42;

    volatile uint8_t lock_c = 0;
    volatile uint8_t lock_asm = 0;
    volatile uint8_t lock_std = 0;

    printf("--- TAS Consistency Examples ---\n\n");

    printf("Executing example with Pure C (Non-Atomic)...\n");
    bool res_c = example(&data_c, &lock_c, test_n_set_c);
    printf(
        "Status: %s, Stored Data: %d, Lock: %u\n\n",
        res_c ? "SUCCESS" : "LOCKED",
        data_c,
        lock_c
    );

    printf("Executing example with x86 Assembly (Atomic)...\n");
    bool res_asm = example(&data_asm, &lock_asm, test_n_set_asm);
    printf(
        "Status: %s, Stored Data: %d, Lock: %u\n\n",
        res_asm ? "SUCCESS" : "LOCKED",
        data_asm,
        lock_asm
    );

    printf("Executing example with C11 stdatomic (Atomic)...\n");
    bool res_std = example(&data_std, &lock_std, test_n_set_stdatomic);
    printf(
        "Status: %s, Stored Data: %d, Lock: %u\n\n",
        res_std ? "SUCCESS" : "LOCKED",
        data_std,
        lock_std
    );

    return EXIT_SUCCESS;
}
