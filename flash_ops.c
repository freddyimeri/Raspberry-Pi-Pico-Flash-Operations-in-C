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
 * Write data safely to the flash memory at a specified offset ensuring that all parameters and alignment rules
 * are strictly adhered to in order to prevent data corruption and adhere to device specifications.
 * 
 * @param offset The offset from the base where data starts to be written in the flash memory.
 * @param data Pointer to the data buffer to be written to flash.
 * @param data_len The length of the data to be written in bytes.
 */
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len) {
    
    // Calculate the actual flash memory address by adding the target offset to the base address.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    printf("flash_offset: %d\n", flash_offset);

    // Ensure the data pointer is not NULL and the data length is greater than zero.
    if (data == NULL || data_len == 0) {
        printf("Error: No data provided or data length is zero.\n");
        return;  // Exit if data is invalid, preventing further operations.
    }

    // Check if the provided offset aligns with the flash sector size to prevent cross-sector issues.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for write. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return;
    }

    // Verify that the data length does not exceed the limits of a single sector minus the space needed for metadata.
    if (data_len > (FLASH_SECTOR_SIZE - METADATA_SIZE)) {
        printf("Error: Data size exceeds the maximum allowed limit per sector (%u bytes allowed).\n", FLASH_SECTOR_SIZE - METADATA_SIZE);
        return;
    }

    // Check to ensure that the write operation does not go beyond the physical memory limits of the flash.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to write beyond flash memory limits.\n");
        return;
    }


    uint32_t initial_count = get_flash_write_count(offset);
    initial_count = initial_count + 1;

    flash_data flashData = {
        .valid = true,
        .write_count = initial_count,
        .data_len = data_len,
        .data_ptr = data
    };

    

    size_t total_size = sizeof(flash_data) + flashData.data_len;  // Calculate total required size


    uint8_t *flash_data_buffer = malloc(total_size);


    if (!flash_data_buffer) {
        printf("Failed to allocate memory for flash data buffer.\n");
        return;
    }
    serialize_flash_data(&flashData, flash_data_buffer, total_size);


    // Save the current state of interrupts and disable them to prevent interference 
    // during the flash write operation, ensuring atomicity of the write process.
    uint32_t ints = save_and_disable_interrupts();


    // Erase the flash sector at the specified offset to prepare it for a clean write.
    // This is necessary because flash memory must be erased before new data can be programmed.
    flash_range_erase(offset, FLASH_SECTOR_SIZE);

    // Write the new structured data to flash, including metadata such as write count,
    // ensuring all information is stored accurately.
    flash_range_program(offset, flash_data_buffer, total_size);

    // Restore the interrupts to their previous state after the write operation is complete,
    // ensuring the system's interrupt configuration is maintained correctly.
    restore_interrupts(ints);
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
  

    // Ensure the offset is aligned with the flash sector size to prevent partial sector read issues.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for read. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return;
    }

    // Verify the requested read does not exceed the flash memory's bounds.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return;
    }

    const size_t total_size = sizeof(flash_data) + buffer_len;  // Calculate total required size

    uint8_t *flash_data_buffer = malloc(total_size);

    if (flash_data_buffer == NULL) {
        printf("Failed to allocate memory for flash data buffer.\n");
        return;
    }

    memcpy(flash_data_buffer, (const void *)(XIP_BASE + offset), total_size);


    flash_data data;
    deserialize_flash_data(flash_data_buffer, &data);


    memcpy(buffer, data.data_ptr, buffer_len);


}




  
 
/**
 * Erase a sector of the flash memory at a specified offset. This function ensures
 * that the operation respects flash memory boundaries and alignment requirements,
 * preserving essential metadata while erasing the rest of the sector.
 *
 * @param offset The offset within the flash memory where the sector begins to be erased.
 */
void flash_erase_safe(uint32_t offset) {

    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    

    // Check if the offset is aligned with the flash sector size to ensure that entire
    // sectors are erased, which is a requirement for most flash memory devices.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for erase. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return; // Prevent erasing if the offset is not properly aligned.
    }

    // Verify that the erasing operation does not extend beyond the available flash memory space.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to erase beyond flash memory limits.\n");
        return; // Stop operation to avoid memory corruption.
    }

    uint32_t initial_count = get_flash_write_count(offset);
    initial_count = initial_count + 1;
    
    
    uint32_t ints = save_and_disable_interrupts(); // Disable interrupts to ensure the operation's atomicity.

    // Calculate the actual start of the sector to ensure the entire sector is correctly erased.
    uint32_t sector_start = flash_offset & ~(FLASH_SECTOR_SIZE - 1);

    // Check if the computed sector start is beyond the flash memory's boundary.
    if (sector_start >= FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Sector start address is out of bounds.\n");
        restore_interrupts(ints);
        return; // Restore interrupts and exit if the sector start is invalid.
    }

    // Backup existing metadata before erasing to preserve essential data like write counts.
    flash_data metadata_backup;
    memcpy(&metadata_backup, (const void*)(XIP_BASE + sector_start), sizeof(flash_data));

    // Perform the erase operation on the calculated sector.
    flash_range_erase(sector_start, FLASH_SECTOR_SIZE);

    // Prepare metadata to be restored after erasing.
    flash_data metadata_to_restore = {
        .valid = false,  // Mark as invalid since data is erased.
        .write_count = initial_count,  // Preserve the write count.
        .data_len = 0,  // Reset data length to zero as data is no longer valid.
        .data_ptr = NULL  // Clear data pointer.
    };


    // Restore the metadata to flash after erasing the sector.
    flash_range_program(sector_start, (const uint8_t *)&metadata_to_restore, sizeof(flash_data));

    restore_interrupts(ints); // Re-enable interrupts after the operation is complete.
}


