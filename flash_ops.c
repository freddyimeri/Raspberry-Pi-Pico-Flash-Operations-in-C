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
#include <stdio.h>
#include <string.h>
 
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>
 #include <stdbool.h>  // Include this header for bool type

#define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts (256KB into flash)
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

    // Initialize flash data structure with the provided parameters.
    flash_data flashData;
    flashData.write_count = 0;  // Set initial write count to 0.
    flashData.data_ptr = data;  // Set data pointer to the provided data.
    flashData.data_len = data_len;  // Set data length to the provided length.

    // Call the helper function to perform the actual structured write to flash.
    flash_write_safe_struct(flash_offset, &flashData);
}



/**
 * Writes structured data to a specific offset in flash memory after handling data integrity and
 * management operations. This function ensures data consistency and manages the write count to
 * track flash memory wear.
 * 
 * @param offset   The offset in flash memory where the data is to be written.
 * @param new_data Pointer to the structured data containing the information to be written.
 */
void flash_write_safe_struct(uint32_t offset, flash_data *new_data) {
    
    // Save the current state of interrupts and disable them to prevent interference 
    // during the flash write operation, ensuring atomicity of the write process.
    uint32_t ints = save_and_disable_interrupts();

    // Initialize a temporary structure to hold the data currently stored at the flash location.
    flash_data current_data;
    memset(&current_data, 0, METADATA_SIZE);

    // Read the existing data at the specified flash offset into the temporary structure.
    memcpy(&current_data, (const void *)(XIP_BASE + offset), METADATA_SIZE);
  
    // If no valid entry exists yet, initialize the write count to zero.
    if(current_data.write_count == -1) {
        current_data.write_count = 0;
    }

    // Increment the write count to reflect this new write operation, tracking how many
    // times this particular flash location has been written to, which can help with wear leveling.
    new_data->write_count = current_data.write_count + 1;

    // Mark the new data as valid before writing it to flash.
    new_data->valid = true;

    // Erase the flash sector at the specified offset to prepare it for a clean write.
    // This is necessary because flash memory must be erased before new data can be programmed.
    flash_range_erase(offset, FLASH_SECTOR_SIZE);

    // Write the new structured data to flash, including metadata such as write count,
    // ensuring all information is stored accurately.
    flash_range_program(offset, (const uint8_t *)new_data, METADATA_SIZE);

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
    printf("flash_offset: %d\n", flash_offset);

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

    // Structure to hold the data read from flash, initialized to zero.
    flash_data flashData;
    memset(&flashData, 0, sizeof(flashData));

    // Perform the structured read from flash.
    flash_read_safe_struct(flash_offset, &flashData);
    
    // Check if the retrieved data is valid and initialized.
    if (!flashData.valid || flashData.data_len == 0) {
        printf("Error: Attempt to read invalid or uninitialized flash data.\n");
        return;
    }

    // Adjust buffer_len to be no larger than the actual data length to avoid overflows.
    if (buffer_len > flashData.data_len) {
        buffer_len = flashData.data_len;
    }
    
    // Copy the data from the flash memory to the provided buffer.
    // Note: data_ptr must be handled by a specific implementation as it cannot directly point to a meaningful location.
    memcpy(buffer, flashData.data_ptr, buffer_len);
    printf("Flash data is valid and read successfully.\n");
}



/**
 * Reads structured data from a specific offset within the flash memory. This function ensures
 * that the read operation does not exceed the memory bounds of the flash and transfers the
 * data into a provided structure.
 * 
 * @param offset The offset within the flash memory from which to read the data.
 * @param data   Pointer to a structure where the read flash data should be stored.
 */
void flash_read_safe_struct(uint32_t offset, flash_data *data) {
    // Ensure that the read operation does not go beyond the allocated flash memory.
    // This checks if the end of the data that would be read is within the flash memory boundaries.
    if (offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return; // Exit the function if the memory access would be out of bounds.
    }

    // Execute the memory copy operation to transfer the data from flash to the provided data structure.
    // XIP_BASE is the base address of the externally mapped flash memory, allowing direct memory access.
    memcpy(data, (const void *)(XIP_BASE + offset), METADATA_SIZE);
}

  
 
/**
 * Erase a sector of the flash memory at a specified offset. This function ensures
 * that the operation respects flash memory boundaries and alignment requirements,
 * preserving essential metadata while erasing the rest of the sector.
 *
 * @param offset The offset within the flash memory where the sector begins to be erased.
 */
void flash_erase_safe(uint32_t offset) {
    printf("offset: %d\n", offset);
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    printf("flash_offset: %d\n", flash_offset);

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

    printf("flash_offset: %d\n", flash_offset);
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
        .write_count = metadata_backup.write_count,  // Preserve the write count.
        .data_len = 0,  // Reset data length to zero as data is no longer valid.
        .data_ptr = NULL  // Clear data pointer.
    };

    printf("metadata_to_restore.valid: %d\n", metadata_to_restore.valid);
    // Restore the metadata to flash after erasing the sector.
    flash_range_program(sector_start, (const uint8_t *)&metadata_to_restore, sizeof(flash_data));

    restore_interrupts(ints); // Re-enable interrupts after the operation is complete.
}



/**
 * Retrieves the write count for a specific sector in the flash memory. This function checks
 * that the given offset aligns with the flash sector size and is within the flash memory's
 * boundaries before accessing the flash data.
 *
 * @param offset The offset from the start of the flash memory for which to retrieve the write count.
 * @return The number of times the flash sector at the given offset has been written. Returns 0
 *         if there is an error due to misalignment or boundary overflow, or if the sector has not
 *         been written yet.
 */
uint32_t get_flash_write_count(uint32_t offset) {
    // Calculate the actual memory address within the flash memory where the write count is stored.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

    // Ensure that the offset is aligned with the flash sector size to prevent reading from an incorrect sector.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for getting the Write count. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return 0; // Return 0 as an error indicator due to misalignment.
    }

    // Check to ensure that the read operation stays within the bounds of the flash memory to avoid overflow errors.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read for write count beyond flash memory limits.\n");
        return 0; // Return 0 as an error indicator due to attempting to read beyond the flash memory limits.
    }

    // Define a temporary structure to store the data read from flash memory.
    flash_data tempFlashData;

    // Read the metadata from the specified offset within the flash memory.
    memcpy(&tempFlashData, (const void *)(XIP_BASE + flash_offset), METADATA_SIZE);

    // Return the retrieved write count. This count helps in understanding the wear level of the flash sector.
    return tempFlashData.write_count;
}



/**
 * Retrieves the data length for a specific sector in the flash memory. This function ensures
 * that the read operation respects alignment with the flash sector size and stays within the
 * memory boundaries, crucial for accurate data retrieval and system stability.
 *
 * @param offset The offset from the start of the flash memory from which to retrieve the data length.
 * @return The length of the data stored at the given offset, in bytes. Returns 0 if there is an
 *         error due to misalignment, boundary issues, or if the sector has not been initialized.
 */
uint32_t get_flash_data_length(uint32_t offset) {
    // Calculate the actual memory address in flash memory.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

    // Ensure the offset is aligned with the flash sector size to avoid reading incomplete or incorrect data.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for getting data length. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return 0; // Return 0 to indicate an error due to misalignment.
    }

    // Check that the memory address for reading is within the allowed flash memory bounds.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read for data length beyond flash memory limits.\n");
        return 0; // Return 0 to indicate an error due to reading beyond flash memory limits.
    }

    // Define a temporary structure to hold the flash data read from memory.
    flash_data tempFlashData;

    // Read the data from the specified flash memory offset.
    memcpy(&tempFlashData, (const void *)(XIP_BASE + flash_offset), METADATA_SIZE);

    // Output the data length for debugging and verification purposes.
    printf("FLASH DATA LENGTH: %zu\n", tempFlashData.data_len);

    // Return the data length retrieved from the flash memory.
    return tempFlashData.data_len;
}
