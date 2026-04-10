/**
 * University of La Laguna
 * Higher School of Engineering and Technology
 * Project: Memory Consistency Examples
 * @file cmp_n_swap.c
 * @author Domenico Goya (wh0crypt@computer.org)
 * @brief Demonstration of Compare-and-Swap (CAS) primitives.
 *
 * This program illustrates the difference between software-based
 * comparison and hardware-atomic comparison using the x86 LOCK prefix.
 * It also illustrates the implementation and behavior of the
 * Compare-and-Swap synchronization primitive using three different
 * approaches: standard C (non-atomic), C11 stdatomic, and x86 inline assembly.
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

typedef int (*lock_func_t)(int *, int, int);

/**
 * @brief Atomic Compare-and-Swap using the C11 standard library.
 *
 * Leverages the C11 memory model to perform a strong compare-and-exchange
 * operation. This version is portable and ensures sequential consistency
 * by default.
 *
 * @param value Pointer to the atomic integer to be modified.
 * @param expected Pointer to the value expected to be found in 'value'.
 * @param new_value The value to write into 'value' if the comparison succeeds.
 * @return int The value of 'value' before the operation..
 */
static inline int
cmp_n_swap_stdatomic(int *value, int expected, int new_value) {
    int exp_val = expected;
    atomic_compare_exchange_strong((_Atomic int *)value, &expected, new_value);
    return exp_val;
}

/**
 * @brief Non-atomic Compare-and-Swap implementation in pure C.
 *
 * @warning This function is NOT thread-safe and lacks atomicity.
 * It is intended only for educational purposes to demonstrate how
 * a CAS operation would look in high-level logic without hardware locks.
 *
 * @param value Pointer to the integer variable.
 * @param expected The value expected to be in 'value'.
 * @param new_value The value to write if 'value' matches 'expected'.
 * @return int The value of 'value' before the operation started.
 */
static inline int cmp_n_swap_c(int *value, int expected, int new_value) {
    int temp = *value;
    if (*value == expected) {
        *value = new_value;
    }
    return temp;
}

/**
 * @brief Atomic Compare-and-Swap (CAS) implementation using x86 assembly.
 *
 * Performs an atomic comparison of the destination operand (value) with
 * the accumulator register (EAX). If the values are equal, the source
 * operand (new_value) is loaded into the destination. Otherwise, the
 * destination is loaded into the accumulator.
 *
 * The 'lock' prefix ensures the instruction has exclusive access to the
 * shared memory, providing atomicity across multiple processor cores.
 *
 * @param value Pointer to the destination memory.
 * @param expected The value we believe is currently there.
 * @param new_value The value to write if the comparison succeeds.
 * @return The value that was in 'value' before the operation.
 */
static inline int cmp_n_swap_asm(int *value, int expected, int new_value) {
    int actual;
    __asm__ volatile(
        "lock cmpxchg [%1], %2"
        : "=a"(actual)    /* The result from the EAX register after execution */
        : "r"(value),     /* Pointer passed in any general-purpose register */
          "r"(new_value), /* The new value to be set */
          "a"(expected)   /* Initial value for EAX (required by cmpxchg) */
        : "memory"        /* Forces the compiler to flush registers to memory
                             before/after */
    );
    return actual;
}

/**
 * @brief Simulates a thread-safe increment using a CAS loop.
 *
 * This function demonstrates the optimistic concurrency control pattern.
 * It reads the current value, performs a calculation, and attempts to
 * commit the change only if the value hasn't been modified by another
 * thread in the meantime.
 *
 * @param x Pointer to the shared resource.
 * @param lock_func The CAS implementation to use.
 * @return int The final value successfully written to memory.
 */
int example(int *x, lock_func_t lock_func) {
    int current_val;
    int new_val;

    /**
     * CAS Loop: Keep trying until the swap succeeds.
     * In a real multithreaded environment, this handles contention.
     */
    do {
        current_val = *x;
        new_val = current_val + 10;
    } while (lock_func(x, current_val, new_val) != current_val);

    return new_val;
}

int main(void) {
    int data_c = 100;
    int data_asm = 100;
    int data_std = 100;

    printf("--- CAS Consistency Examples ---\n\n");

    printf("Executing example with Pure C (Non-Atomic)...\n");
    int res_c = example(&data_c, cmp_n_swap_c);
    printf("Result: %d, Memory: %d\n\n", res_c, data_c);

    printf("Executing example with x86 Assembly (Atomic LOCK CMPXCHG)...\n");
    int res_asm = example(&data_asm, cmp_n_swap_asm);
    printf("Result: %d, Memory: %d\n\n", res_asm, data_asm);

    printf("Executing example with C11 stdatomic (Atomic)...\n");
    int res_std = example(&data_std, cmp_n_swap_stdatomic);
    printf("Result: %d, Memory: %d\n", res_std, data_std);

    return EXIT_SUCCESS;
}
