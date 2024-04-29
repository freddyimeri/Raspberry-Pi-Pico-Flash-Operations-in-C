#include "test.h"
#include "flash_ops.h"
#include <stdio.h>
#include <string.h>
 // test if flash_write safe takes an empty data, fix on the code
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
 
// make a variable that contains slashes and then use it to make the code more readable


#define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts (256KB into flash)
#define FLASH_SIZE PICO_FLASH_SIZE_BYTES // Total flash size available
#define METADATA_SIZE sizeof(flash_data) 



void run_all_tests() {
    char slashes[] = "\n/////////////////////////////////////////////\n";
    // test_write_aligned_and_within_bounds();
    // test_erase_aligned();
    // test_full_cycle_operation();


    printf("%s\n", slashes);
    
    test_unaligned_offset();
    printf("%s\n", slashes);

    test_flash_write_count_persistence();
    printf("%s\n", slashes);


    test_data_length_retrieval();  // New test declaration
    printf("%s\n", slashes);

    test_flash_beyond_flash_limits();
    printf("%s\n", slashes);


    test_null_or_zero_data();
    printf("%s\n", slashes);

    test_exceed_sector_size();  
    printf("%s\n", slashes);

    

    // test_full_cycle_operation();
    // printf("%s\n", slashes);
    
    
    

}

void test_data_length_retrieval() {
    printf("Testing retrieval of data length from flash...\n");
    uint32_t offset = 4096; // Correctly aligned offset
    uint8_t data[150]; // Example data
    memset(data, 0xAB, sizeof(data)); // Fill the data with a pattern

    // Write data to flash
    printf("Writing data to flash...\n");
    flash_write_safe(offset, data, sizeof(data));

    // Get data length from flash
    uint32_t retrieved_length = get_flash_data_length(offset); ////////////////////////////////////////////////////////////////
    printf("Retrieved data length: %u\n", retrieved_length);

    // Verify the retrieved data length matches what was written
    if (retrieved_length == sizeof(data)) {
        printf("PASS: Retrieved data length is correct.\n");
    } else {
        printf("FAIL: Retrieved data length is incorrect (expected: %zu, got: %u).\n", sizeof(data), retrieved_length);
    }

   
    printf("Erasing the sector...\n");
    flash_erase_safe(offset);

    // Verify data length is reset to 0 after erase
    retrieved_length = get_flash_data_length(offset);
    printf("Retrieved data length after erase: %u\n", retrieved_length);
    if (retrieved_length == 0) {
        printf("PASS: Data length correctly reset to 0 after erase.\n");
    } else {
        printf("FAIL: Data length not reset after erase (expected: 0, got: %u).\n", retrieved_length);
    }

}

void test_flash_write_count_persistence() {
    printf("Testing persistence of write count through write and erase cycles...\n");
    
    uint32_t offset = 4096; // Correctly aligned offset
    uint8_t data[100]; // Example data
    memset(data, 0xAB, sizeof(data)); // Fill the data with a pattern

    // Initial write
    printf("Initial write...\n");
    flash_write_safe(offset, data, sizeof(data));
    uint32_t initial_count = get_flash_write_count(offset);

    // Erase the sector
    printf("Erasing the sector...\n");
    flash_erase_safe(offset);
    
    // Another write after erase
    printf("Writing after erase...\n");
    flash_write_safe(offset, data, sizeof(data));
    uint32_t second_count = get_flash_write_count(offset);

    // Verify if the write count has incremented correctly
    printf("Verifying write count persistence...\n");
    if (second_count == initial_count + 1) {
        printf("PASS: Write count persisted and incremented correctly after erase (initial: %u, after: %u).\n", initial_count, second_count);
    } else {
        printf("FAIL: Write count did not increment correctly (initial: %u, after: %u).\n", initial_count, second_count);
    }
}


void test_flash_beyond_flash_limits() {
    printf("Testing flash functions with offset and metadata size exceeding flash memory limits...\n");
    uint32_t offset = FLASH_SIZE + FLASH_SECTOR_SIZE;  // Deliberately beyond flash limits
    uint8_t data[10];  // Small data array to keep the focus on the offset calculation
    memset(data, 0xEE, sizeof(data));  // Fill the data array with some pattern

    // Attempt to write data to the flash at an offset that should exceed the memory limits
    flash_write_safe(offset, data, sizeof(data));



    uint8_t buffer[sizeof(data)] = {0}; // Buffer to read back data
    memset(buffer, 0, sizeof(buffer));  // Initialize buffer to zeros
    // Attempt to read data back from the same offset
    flash_read_safe(offset, buffer, sizeof(buffer));

    // attemp to erase the data
    flash_erase_safe(offset);


    uint32_t write_count = get_flash_write_count(offset);


    uint32_t retrieved_length = get_flash_data_length(offset);

    // The function flash_write_safe should automatically print an error due to exceeding memory limits
}




void test_exceed_sector_size() {
    printf("Testing flash_write_safe with data size exceeding sector limits minus metadata...\n");
    uint32_t offset = 4096;  // Correctly aligned offset

    // Excessively large data size: FLASH_SECTOR_SIZE - METADATA_SIZE + 1 should fail
    uint8_t data[FLASH_SECTOR_SIZE - METADATA_SIZE + 1];
    memset(data, 0xCD, sizeof(data));  // Fill the data array with some pattern

    // Attempt to write data to the flash
    flash_write_safe(offset, data, sizeof(data));

    
    // The function flash_write_safe should automatically print an error due to excessive data size
}



void test_unaligned_offset() {
    printf("Testing functions with unaligned offset...\n");
    uint32_t offset = 6096;  // This offset is not aligned with 4096-byte sectors
    uint8_t data[100];       // Example data buffer to attempt to write
    memset(data, 0xAB, sizeof(data));  // Fill the data array with a pattern

    uint8_t buffer[sizeof(data)] = {0}; // Buffer to read back data
    memset(buffer, 0, sizeof(buffer));  // Initialize buffer to zeros

    // Attempt to write data to the unaligned offset
    flash_write_safe(offset, data, sizeof(data));

    // Attempt to read data back from the same offset
    flash_read_safe(offset, buffer, sizeof(buffer));

    // attemp to erase the data
    flash_erase_safe(offset);

 
    uint32_t write_count = get_flash_write_count(offset);

    uint32_t retrieved_length = get_flash_data_length(offset);
    
}


void test_full_cycle_operation() {
    printf("Testing write, read, and erase cycle...\n");
    
    // Setup
    uint32_t offset = 4096;  // Correctly aligned offset
    uint8_t write_data[100]; // Data to write
    memset(write_data, 0xA5, sizeof(write_data));  // Fill the data array with some pattern

    // Write data to flash
    flash_write_safe(offset, write_data, sizeof(write_data));
    
    // Read data back from flash
    uint8_t read_data[sizeof(write_data)];
    memset(read_data, 0, sizeof(read_data));
    flash_read_safe(offset, read_data, sizeof(read_data));

    // Check if the read data matches the written data
    if (memcmp(write_data, read_data, sizeof(write_data)) == 0) {
        printf("PASS: Data written and read back correctly.\n");
    } else {
        printf("FAIL: Data mismatch between written and read data.\n");
    }

    // Erase the data written
    flash_erase_safe(offset);

    // Optionally, check if data is erased if function to read raw flash is available
    // This part depends on the availability of a function to directly read flash memory to verify erasure
    printf("Data erased. (Note: Verification of erasure not performed in this test.)\n");
}


void test_null_or_zero_data() {
    printf("Testing flash_write_safe with NULL data and zero data length...\n");

    uint32_t offset = 4096;  // Correctly aligned offset

    // Test with NULL data
    printf("Test with NULL data:\n");
    flash_write_safe(offset, NULL, 100);  // Expecting an error message about NULL data

    // Test with zero data length
    printf("Test with zero data length:\n");
    uint8_t data[10];   // Small data array 
    flash_write_safe(offset, data, 0);  // Expecting an error message about zero data length
}



void test_write_aligned_and_within_bounds() {
    printf("Testing flash_write_safe with aligned offset and data within bounds...\n");
    uint8_t data[100];  // Example data, size well within the bounds of FLASH_SECTOR_SIZE - METADATA_SIZE
    flash_write_safe(4096, data, sizeof(data));  // Assuming 4096 is within FLASH_TARGET_OFFSET and properly aligned
}

void test_erase_aligned() {
    printf("Testing flash_erase_safe with aligned offset...\n");
    flash_erase_safe(4096);  // Assuming 4096 is within FLASH_TARGET_OFFSET and properly aligned
}