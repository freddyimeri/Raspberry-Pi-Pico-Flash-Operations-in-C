/**
 * @file test.h
 * 
 * Header file for defining test functions for the flash memory operations on the Raspberry Pi Pico.
 * This file includes declarations for a suite of tests that verify the integrity, reliability,
 * and correctness of the flash memory operations defined in flash_ops.h. These tests are designed
 * to cover a wide range of scenarios including edge cases, regular operations, and error handling
 * capabilities of the flash operations.
 *
 * The functions in this file are used to conduct comprehensive testing to ensure that the flash
 * memory management functionality behaves as expected under various conditions.
 */

#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stddef.h>

// Declaration of function that orchestrates the execution of all defined tests.
void run_all_tests();

// Test function for verifying the full cycle of write, read, and erase operations.
void test_full_cycle_operation();

// Test function for handling of unaligned memory offsets during flash operations.
void test_unaligned_offset();

// Test function for data sizes that exceed the permissible sector size minus metadata.
void test_exceed_sector_size();

// Test function for operations that attempt to go beyond the flash memory limits.
void test_flash_beyond_flash_limits();

// Test function for verifying the persistence and accuracy of the write count in flash memory.
void test_flash_write_count_persistence();

// Test function for checking the accuracy of data length retrieval from flash memory.
void test_data_length_retrieval();

// Test function for handling scenarios where no data or zero-length data is provided to write functions.
void test_null_or_zero_data();


void test_save_and_recover_struct();

#endif // TEST_H
