#ifndef TEST_H
#define TEST_H

#include <stdint.h>
#include <stddef.h>

// Function declarations for the test cases
void test_flash_write_alignment();
void test_flash_write_bounds();
void test_flash_erase_safe();
void run_all_tests();

// External variables for tracking test results
extern int tests_passed;
extern int tests_failed;

#endif // TEST_H
