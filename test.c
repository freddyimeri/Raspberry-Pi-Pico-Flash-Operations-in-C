/**
 * @file test.c
 * 
 * This file contains a suite of tests designed to verify the functionality and robustness
 * of the flash memory operations on the Raspberry Pi Pico. The tests cover a variety
 * of scenarios including writing, reading, erasing, and boundary conditions to ensure
 * that the flash memory operations handle all expected and edge cases correctly. These
 * tests are crucial for ensuring the reliability and stability of flash memory management
 * in embedded systems.
 *
 * Each test function within this file is designed to check specific aspects of the flash
 * operations implemented in flash_ops.h, providing a comprehensive evaluation of each
 * function's behavior under normal and extreme conditions.
 */
#include "flash_ops_helper.h"
#include "test.h"
#include "flash_ops.h"
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
 
 

#define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts (256KB into flash)
#define FLASH_SIZE PICO_FLASH_SIZE_BYTES // Total flash size available
#define METADATA_SIZE sizeof(flash_data) 



/**
 * Executes a series of test functions designed to validate the functionality and robustness
 * of flash memory operations. Each test is separated by printed slashes for clear visual
 * separation of results in the console output.
 */
void run_all_tests() {
    char slashes[] = "\n/////////////////////////////////////////////\n";
    printf("%s\n", slashes);
    
    // Test handling of unaligned memory offset.
    test_unaligned_offset();
    printf("%s\n", slashes);

    // Test the persistence of the write count through write and erase cycles.
    test_flash_write_count_persistence();
    printf("%s\n", slashes);

    // Test the accuracy of data length retrieval from flash memory.
    test_data_length_retrieval();  // New test declaration
    printf("%s\n", slashes);

    // Test handling of operations beyond the flash memory's limits.
    test_flash_beyond_flash_limits();
    printf("%s\n", slashes);

    // Test behavior with null data and zero data length.
    test_null_or_zero_data();
    printf("%s\n", slashes);

    // Test behavior with data sizes exceeding sector size minus metadata.
    test_exceed_sector_size();  
    printf("%s\n", slashes);

    // Test a complete cycle of write, read, and erase operations.
    test_full_cycle_operation();
    printf("%s\n", slashes);


    test_save_and_recover_struct();
    printf("%s\n", slashes);
}


/**
 * Tests the accuracy and reliability of retrieving the data length from flash memory after writing
 * and ensures that the data length is reset to zero after erasing the sector. This test helps verify
 * that the data length metadata is being correctly updated and cleared as expected.
 */
void test_data_length_retrieval() {
    printf("Testing retrieval of data length from flash...\n");
    uint32_t offset = 4096; // Correctly aligned offset for the test.
    uint8_t data[150]; // Example data buffer to be written to flash.
    memset(data, 0xAB, sizeof(data)); // Fill the data buffer with a pattern for easy tracking.

    // Write data to flash at the specified offset and check the operation's success.
    printf("Writing data to flash...\n");
    flash_write_safe(offset, data, sizeof(data));

    // Retrieve the length of data stored at the specified offset after the write operation.
    uint32_t retrieved_length = get_flash_data_length(offset); 
    printf("Retrieved data length: %u\n", retrieved_length);

    // Compare the retrieved data length to the expected size to verify correct handling.
    if (retrieved_length == sizeof(data)) {
        printf("PASS: Retrieved data length is correct.\n");
    } else {
        printf("FAIL: Retrieved data length is incorrect (expected: %zu, got: %u).\n", sizeof(data), retrieved_length);
    }

    // Erase the sector where data was previously written.
    printf("Erasing the sector...\n");
    flash_erase_safe(offset);

    // Retrieve the data length again after erasing the sector to ensure it has been reset.
    retrieved_length = get_flash_data_length(offset);
    printf("Retrieved data length after erase: %u\n", retrieved_length);
    if (retrieved_length == 0) {
        printf("PASS: Data length correctly reset to 0 after erase.\n");
    } else {
        printf("FAIL: Data length not reset after erase (expected: 0, got: %u).\n", retrieved_length);
    }
}



/**
 * Tests the persistence and accuracy of the write count for a flash memory sector.
 * This function performs a series of operations including writing, erasing, and rewriting
 * to the same sector to ensure that the write count is maintained and incremented correctly.
 * It is crucial for evaluating the reliability of wear leveling mechanisms and data integrity.
 */
void test_flash_write_count_persistence() {
    printf("Testing persistence of write count through write and erase cycles...\n");
    
    // Setup: Choose a sector-aligned offset and prepare test data.
    uint32_t offset = 4096; // Correctly aligned offset for testing.
    uint8_t data[100]; // Example data buffer to use in test.
    memset(data, 0xAB, sizeof(data)); // Fill the data buffer with a pattern to identify it easily.

    // Perform the initial write operation and retrieve the write count.
    printf("Initial write...\n");
    flash_write_safe(offset, data, sizeof(data));
    uint32_t initial_count = get_flash_write_count(offset); // Record the write count after the first write.

    // Erase the sector to simulate a typical usage scenario of write-erase cycles.
    printf("Erasing the sector...\n");
    flash_erase_safe(offset);
    
    // Perform another write operation after erasing the sector.
    printf("Writing after erase...\n");
    flash_write_safe(offset, data, sizeof(data));
    uint32_t second_count = get_flash_write_count(offset); // Record the write count after the second write.

    // Verify if the write count has incremented correctly, indicating reliable tracking.
    printf("Verifying write count persistence...\n");
    if (second_count == initial_count + 1) {
        printf("PASS: Write count persisted and incremented correctly after erase (initial: %u, after: %u).\n", initial_count, second_count);
    } else {
        printf("FAIL: Write count did not increment correctly (initial: %u, after: %u).\n", initial_count, second_count);
    }
}




/**
 * Tests flash memory operations (write, read, and erase) with offsets that exceed the
 * defined flash memory boundaries to ensure that error handling and boundary checks
 * are functioning correctly. This test helps verify the robustness and stability of
 * the flash memory management functions.
 */
void test_flash_beyond_flash_limits() {
    printf("Testing flash functions with offset and metadata size exceeding flash memory limits...\n");

    // Set an offset beyond the flash memory limits to test boundary checks.
    uint32_t offset = FLASH_SIZE + FLASH_SECTOR_SIZE;  // Deliberately beyond flash limits

    // Prepare a small data array to keep the focus on the offset calculation rather than data size.
    uint8_t data[10];
    memset(data, 0xEE, sizeof(data));  // Fill the data array with a recognizable pattern to trace in memory if needed.

    // Attempt to write data to the flash at an offset that should exceed the memory limits.
    // This should trigger an error message from the flash_write_safe function due to boundary checking.
    flash_write_safe(offset, data, sizeof(data));

    // Initialize a buffer to attempt to read back any data from the same offset.
    uint8_t buffer[sizeof(data)] = {0};
    memset(buffer, 0, sizeof(buffer));  // Ensure buffer is clean before attempting read.

    // Attempt to read data from the same offset that exceeds the flash memory limits.
    // This should also trigger an error message from the flash_read_safe function.
    flash_read_safe(offset, buffer, sizeof(buffer));

    // Attempt to erase data at the offset beyond the limits.
    // This operation should also fail and generate an error message due to boundary checks.
    flash_erase_safe(offset);

    // Attempt to retrieve the write count for the sector at the specified offset.
    // Since the operation should not be successful, it should return zero or trigger an error.
    uint32_t write_count = get_flash_write_count(offset);

    // Attempt to retrieve the data length at the same invalid offset.
    // This function should also return zero or an appropriate error message if boundaries are respected.
    uint32_t retrieved_length = get_flash_data_length(offset);

    
}





/**
 * Tests the behavior of the flash_write_safe function when attempting to write data that
 * exceeds the maximum allowable size for a single sector, adjusted for metadata size. This
 * test ensures that the function properly handles cases where data sizes violate sector
 * boundaries, crucial for maintaining flash memory integrity and avoiding corruption.
 */
void test_exceed_sector_size() {
    printf("Testing flash_write_safe with data size exceeding sector limits minus metadata...\n");
    // Choose a correctly aligned offset to isolate the test case to only examine data size constraints.
    uint32_t offset = 4096;  // Offset aligned with the start of a sector.

    // Prepare a data buffer that is just one byte larger than the maximum allowed size for a sector,
    // which should trigger an error condition in the flash_write_safe function.
    uint8_t data[FLASH_SECTOR_SIZE - METADATA_SIZE + 1];  // Data size set to exceed sector capacity after metadata.
    memset(data, 0xCD, sizeof(data));  // Fill the data array with a recognizable pattern for debugging.

    // Attempt to write data to the flash at the specified offset. This should fail
    // due to the data size exceeding the permissible limit for a single sector.
    flash_write_safe(offset, data, sizeof(data));

    // The function flash_write_safe should automatically print an error message
    // indicating that the data size has exceeded the sector size minus metadata.
    // This output confirms that the function's boundary checks are working as expected.
}




/**
 * Tests the behavior of flash memory operations (write, read, erase) when provided with an
 * unaligned offset. This test is crucial for verifying that all operations correctly handle
 * offsets that do not align with the expected sector boundaries, as required by most flash
 * memory architectures.
 */
void test_unaligned_offset() {
    printf("Testing functions with unaligned offset...\n");
    // Choose an offset that is intentionally not aligned with the 4096-byte sector size.
    uint32_t offset = 6096;  // This offset should cause alignment errors.

    // Prepare a data buffer with a distinctive pattern to track in memory.
    uint8_t data[100];  // Example data buffer to attempt to write
    memset(data, 0xAB, sizeof(data));  // Fill the data array with a pattern

    // Prepare a buffer to attempt to read back any data written to flash.
    uint8_t buffer[sizeof(data)] = {0};  // Buffer for reading back data
    memset(buffer, 0, sizeof(buffer));  // Initialize buffer to zeros to ensure clean start

    // Attempt to write data to the flash memory at the unaligned offset.
    // This operation should fail or handle the alignment issue as designed in the function.
    flash_write_safe(offset, data, sizeof(data));

    // Attempt to read data from the same unaligned offset.
    // This should also manage the alignment issue and either fail gracefully or correct it.
    flash_read_safe(offset, buffer, sizeof(buffer));

    // Attempt to erase the sector at the unaligned offset.
    // The erase operation should handle the unaligned address properly, either by adjusting the offset or failing safely.
    flash_erase_safe(offset);

    // Retrieve the write count from the flash memory at the specified offset.
    // This value helps in understanding how the system handles unaligned writes.
    uint32_t write_count = get_flash_write_count(offset);

    // Retrieve the length of data stored at the offset to check if any unaligned write was partially successful.
    uint32_t retrieved_length = get_flash_data_length(offset);

    // The function calls should demonstrate appropriate error handling or correction of the unaligned offset.
    // Observing outputs and checking return values are essential for verifying the effectiveness of alignment handling.
}


/**
 * Tests the complete cycle of writing to, reading from, and erasing data in flash memory.
 * This test is designed to verify the integrity and reliability of the flash operations by
 * ensuring data written is correctly read back and subsequently erased.
 */
void test_full_cycle_operation() {
    printf("Testing write, read, and erase cycle...\n");
    
    // Setup: Define a specific offset for the test that is aligned with sector boundaries.
    uint32_t offset = 4096;  // Correctly aligned offset
    uint8_t write_data[100]; // Data to write
    memset(write_data, 0xA5, sizeof(write_data));  // Fill the data array with a specific pattern to trace it easily.

    // Write data to flash at the defined offset.
    flash_write_safe(offset, write_data, sizeof(write_data));
    
    // Prepare a buffer to read the data back into, ensuring it is initially cleared.
    uint8_t read_data[sizeof(write_data)];
    memset(read_data, 0, sizeof(read_data));
    flash_read_safe(offset, read_data, sizeof(read_data));

    // Compare the written data with the read data to verify accuracy of the operations.
    if (memcmp(write_data, read_data, sizeof(write_data)) == 0) {
        printf("PASS: Data written and read back correctly.\n");
    } else {
        printf("FAIL: Data mismatch between written and read data.\n");
    }

    // Erase the data at the specified offset, cleaning up after the test.
    flash_erase_safe(offset);
    printf("Data erased\n");
  
    
}



/**
 * Tests the flash_write_safe function for its ability to handle edge cases where the data
 * pointer is NULL or the data length is zero. These tests ensure that the function properly
 * manages invalid input without causing crashes or undesirable behaviors.
 */
void test_null_or_zero_data() {
    printf("Testing flash_write_safe with NULL data and zero data length...\n");

    // Setup: Use a predefined, correctly aligned offset for the test.
    uint32_t offset = 4096;  // Offset aligned with the flash sector size.

    // Test case 1: Attempt to write with a NULL data pointer.
    printf("Test with NULL data:\n");
    flash_write_safe(offset, NULL, 100);  // This should trigger an error message due to NULL data.

    // Test case 2: Attempt to write with zero data length.
    printf("Test with zero data length:\n");
    uint8_t data[10];   // Define a small data array to test zero length condition.
    flash_write_safe(offset, data, 0);  // This should trigger an error message due to zero data length.

    // Both cases are designed to verify that the flash_write_safe function does not proceed
    // with operations that are likely to fail or cause undefined behavior due to invalid parameters.
}





 
void test_save_and_recover_struct() {
    uint32_t offset = 61440;   


    DeviceConfig config = {
        .id = 5123,
        .sensor_value = 98.6,
        .name = "Device1"
    };
    
    // Serialize DeviceConfig into a buffer
    uint8_t device_config_buffer[sizeof(DeviceConfig)];
    serialize_device_config(&config, device_config_buffer);
    flash_write_safe(offset, device_config_buffer, sizeof(device_config_buffer));
    const size_t total_size =  sizeof(DeviceConfig);
    uint8_t device_config_buffer_read[sizeof(DeviceConfig)];
    flash_read_safe(offset, device_config_buffer_read, total_size);
    DeviceConfig config11;

    deserialize_device_config(device_config_buffer_read, &config11);

    printf("device ID: %d\n", config11.id);
    printf("device sensor value: %f\n", config11.sensor_value);
    printf("device name: %s\n", config11.name);



}
 