#include "flash_ops.h"
#include <stdio.h>

// Mock functions and any required global variables
int tests_passed = 0;
int tests_failed = 0;

void flash_range_erase(uint32_t address, size_t length) {
    // Simulating erasing operation
    printf("Mock: Erasing flash at address %u for length %zu\n", address, length);
}

void flash_range_program(uint32_t address, const uint8_t *data, size_t length) {
    // Simulating programming operation
    printf("Mock: Programming flash at address %u with data length %zu\n", address, length);
}

void assert_test(int condition, const char *message) {
    if (condition) {
        printf("PASS: %s\n", message);
        tests_passed++;
    } else {
        printf("FAIL: %s\n", message);
        tests_failed++;
    }
}

void test_flash_write_alignment() {
    printf("Testing flash write alignment...\n");
    uint8_t data[100];  // Example data buffer
    // Expected to fail due to incorrect alignment
    flash_write_safe(4095, data, sizeof(data)); 
    assert_test(0, "Write with incorrect alignment should fail.");

    // Expected to pass due to correct alignment
    flash_write_safe(4096, data, sizeof(data));
    assert_test(1, "Write with correct alignment should pass.");
}

void test_flash_write_bounds() {
    printf("Testing flash write bounds...\n");
    uint8_t data[FLASH_SECTOR_SIZE - METADATA_SIZE + 1];  // Slightly too large
    // Expected to fail due to oversized data
    flash_write_safe(0, data, sizeof(data));
    assert_test(0, "Write with oversized data should fail.");

    // Expected to pass because data size is within bounds
    flash_write_safe(0, data, FLASH_SECTOR_SIZE - METADATA_SIZE);
    assert_test(1, "Write within data bounds should pass.");
}

void test_flash_erase_safe() {
    printf("Testing flash erase safety...\n");
    // Expected to pass
    flash_erase_safe(0);
    assert_test(1, "Erase within bounds should pass.");

    // Expected to fail due to out of bounds
    flash_erase_safe(FLASH_SIZE);
    assert_test(0, "Erase out of bounds should fail.");
}

void run_all_tests() {
    test_flash_write_alignment();
    test_flash_write_bounds();
    test_flash_erase_safe();
    printf("\nTotal Tests Passed: %d\nTotal Tests Failed: %d\n", tests_passed, tests_failed);
}

