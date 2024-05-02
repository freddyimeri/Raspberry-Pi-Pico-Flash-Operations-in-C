/**
 * @file flash_ops.c
 * 
 * This module provides a set of functions to perform safe read, write, and erase operations 
 * on the flash memory of a Raspberry Pi Pico. It ensures data integrity and alignment according 
 * to the constraints of the hardware flash memory. Functions in this module handle:
 * 
 * - Alignment checks to ensure all operations respect flash sector boundaries.
 * - Data size checks to prevent buffer overflows and ensure data fits within designated flash sectors.
 * - Boundary checks to prevent operations from exceeding the physical memory limits.
 * - Utilities to read and write structured data to and from the flash memory, maintaining a count 
 *   of write operations to assist with wear leveling strategies if needed.
 * 
 * The functionality encapsulated in this module is critical for maintaining the durability and 
 * reliability of the flash memory in embedded systems, particularly those requiring non-volatile 
 * storage for state retention across power cycles.
 */


#include "flash_ops.h"
#include "flash_ops_helper.h"
#include <stdio.h>
#include <string.h>
 
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>
 #include <stdbool.h>  // Include this header for bool type

#define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts  
#define FLASH_SIZE PICO_FLASH_SIZE_BYTES // Total flash size available
#define METADATA_SIZE sizeof(flash_data)  

/**
 * Write data safely to the flash memory at a specified offset, ensuring that all parameters and alignment rules
 * are strictly adhered to in order to prevent data corruption and adhere to device specifications.
 * 
 * @param offset The offset from the base where data starts to be written in the flash memory.
 * @param data Pointer to the data buffer to be written to flash.
 * @param data_len The length of the data to be written in bytes.
 */
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len) {
    // Calculate the actual flash memory address by adding the target offset to the base address.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    // Print the computed flash memory address for debugging purposes.
    printf("flash_offset: %d\n", flash_offset);

    // Check if data is NULL or if the length is zero, which are invalid inputs.
    if (data == NULL || data_len == 0) {
        printf("Error: No data provided or data length is zero.\n");
        return;  // Exit the function to prevent further operations with invalid data.
    }

    // Ensure the flash offset is a multiple of the flash sector size to avoid cross-sector write issues.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for write. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return;  // Early return if the offset is not aligned.
    }

    // Check if the data size exceeds the sector capacity after accounting for metadata.
    if (data_len > (FLASH_SECTOR_SIZE - METADATA_SIZE)) {
        printf("Error: Data size exceeds the maximum allowed limit per sector (%u bytes allowed).\n", FLASH_SECTOR_SIZE - METADATA_SIZE);
        return;  // Return if the data size is too large for one sector.
    }

    // Prevent writing beyond the physical memory limits of the flash.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to write beyond flash memory limits.\n");
        return;  // Return if the write operation would exceed the flash memory boundaries.
    }

    // Retrieve the current write count for the specified offset, then increment it by one.
    uint32_t initial_count = get_flash_write_count(offset);
    initial_count++;

    // Prepare the flash data structure with new write count and data information.
    flash_data flashData = {
        .valid = true,         // Mark the data as valid.
        .write_count = initial_count, // Updated write count.
        .data_len = data_len,  // Set the length of the data.
        .data_ptr = data       // Point to the data to be written.
    };

    // Calculate the total size required for storing the data and metadata.
    size_t total_size = sizeof(flash_data) + flashData.data_len;

    // Allocate memory for the buffer that will hold both the metadata and the actual data.
    uint8_t *flash_data_buffer = malloc(total_size);
    if (!flash_data_buffer) {
        printf("Failed to allocate memory for flash data buffer.\n");
        return;  // Return if memory allocation fails.
    }

    // Serialize the flashData structure into the allocated buffer.
    serialize_flash_data(&flashData, flash_data_buffer, total_size);

    // Disable interrupts to ensure the flash write operation is not interrupted, maintaining atomicity.
    uint32_t ints = save_and_disable_interrupts();

    // Erase the flash sector before writing new data to ensure it's clean for programming.
    flash_range_erase(offset, FLASH_SECTOR_SIZE);

    // Program the flash memory with new data and metadata.
    flash_range_program(offset, flash_data_buffer, total_size);

    // Restore interrupts to their original state once the flash operation is complete.
    restore_interrupts(ints);

    // Free the allocated buffer after the write operation is done.
    free(flash_data_buffer);
}








/**
 * Read data safely from the flash memory at a specified offset. This function checks
 * alignment and memory bounds to ensure data integrity and prevent memory access errors.
 * It only proceeds with reading if the data is marked valid and properly initialized.
 * 
 * @param offset The offset from the base where data is read in the flash memory.
 * @param buffer The buffer to store read data.
 * @param buffer_len The length of the buffer to ensure no overflow occurs.
 */
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len) {
    // Calculate the actual memory address in flash by adding the base offset.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    // Display calculated flash offset for verification and debugging purposes.
    printf("Calculated flash offset: %d\n", flash_offset);

    // Check if the flash offset is properly aligned with the sector size to avoid misaligned reads.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for read. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return; // Exit function if offset is not aligned.
    }

    // Ensure the read operation does not extend beyond the flash memory's bounds.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return; // Exit function if attempting to read beyond available flash memory.
    }

    // Calculate the total size needed to read, including both metadata and the user data.
    const size_t total_size = sizeof(flash_data) + buffer_len;

    // Allocate a buffer to hold the flash data including the metadata.
    uint8_t *flash_data_buffer = malloc(total_size);
    if (flash_data_buffer == NULL) {
        printf("Failed to allocate memory for flash data buffer.\n");
        return; // Exit if memory allocation fails.
    }

    // Copy the data from flash memory starting at the computed offset into the allocated buffer.
    memcpy(flash_data_buffer, (const void *)(XIP_BASE + offset), total_size);

    // Deserialize the buffer into a flash_data struct to extract metadata and actual data.
    flash_data data;
    deserialize_flash_data(flash_data_buffer, &data);

    // Check if the data is valid before copying it to the user-provided buffer.
    if (data.valid) {
        // Ensure that the buffer is large enough to hold the data.
        if (buffer_len >= data.data_len) {
            // Copy only the amount of data specified in data_len to prevent buffer overflow.
            memcpy(buffer, data.data_ptr, data.data_len);
        } else {
            printf("Error: Buffer provided is too small for the data length.\n");
        }
    } else {
        printf("Error: Invalid data at specified flash offset.\n");
    }

    // Free the allocated buffer after use.
    free(flash_data_buffer);
}




  
 
/**
 * Erase a sector of the flash memory at a specified offset. This function ensures
 * that the operation respects flash memory boundaries and alignment requirements,
 * preserving essential metadata while erasing the rest of the sector.
 *
 * @param offset The offset within the flash memory where the sector begins to be erased.
 */
void flash_erase_safe(uint32_t offset) {
    // Calculate the actual address in flash memory where the erasing will start, including the predefined offset.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

    // Ensure the offset aligns with the sector size to prevent partial erasure of sectors.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for erase. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return; // Exit if the offset is misaligned, as erasing misaligned sectors can lead to data corruption.
    }

    // Check if the erasing would go beyond the limits of the flash memory.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to erase beyond flash memory limits.\n");
        return; // Stop the operation to prevent memory corruption due to out-of-bounds access.
    }

    // Retrieve the current write count for the sector to be erased and increment it to track erase cycles.
    uint32_t initial_count = get_flash_write_count(offset);
    initial_count += 1;

    // Disable interrupts to ensure the erasure process is not interrupted, maintaining the atomicity of the operation.
    uint32_t ints = save_and_disable_interrupts();

    // Compute the exact start of the sector to be erased, ensuring it is rounded down to the nearest sector boundary.
    uint32_t sector_start = flash_offset & ~(FLASH_SECTOR_SIZE - 1);

    // Verify that the calculated sector start does not exceed the flash memory's boundary.
    if (sector_start >= FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Sector start address is out of bounds.\n");
        restore_interrupts(ints);
        return; // Restore interrupts and abort if the start address is invalid.
    }

    // Back up the existing metadata from the sector before erasing to preserve critical data like write counts.
    flash_data metadata_backup;
    memcpy(&metadata_backup, (const void*)(XIP_BASE + sector_start), sizeof(flash_data));

    // Perform the actual erasure of the sector.
    flash_range_erase(sector_start, FLASH_SECTOR_SIZE);

    // Set up metadata for restoration after erasing. Mark data as invalid since it has been erased.
    flash_data metadata_to_restore = {
        .valid = false,
        .write_count = initial_count, // Updated write count after erasure.
        .data_len = 0,
        .data_ptr = NULL
    };

    // Restore the metadata at the start of the erased sector to maintain the integrity of flash management data.
    flash_range_program(sector_start, (const uint8_t *)&metadata_to_restore, sizeof(flash_data));

    // Re-enable interrupts after completing the erasure to restore normal operation.
    restore_interrupts(ints);
}


