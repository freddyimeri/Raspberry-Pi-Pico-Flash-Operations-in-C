#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stddef.h>

// Function declarations for the test cases
void test_write_aligned_and_within_bounds();
void test_erase_aligned();
void run_all_tests();
void test_full_cycle_operation();   

void test_unaligned_offset();  
void test_exceed_sector_size();

void test_flash_beyond_flash_limits();

void test_flash_write_count_persistence();

void test_data_length_retrieval();  

void test_null_or_zero_data();
#endif // TEST_H
