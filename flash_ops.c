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

void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len) {
    
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    printf("flash_offset: %d\n", flash_offset);

     // Check if the offset is aligned with the sector size
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Invalid offset. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return;
    }

     // Check if the data size exceeds the maximum allowed size in a sector minus metadata
    if (data_len > (FLASH_SECTOR_SIZE - METADATA_SIZE)) {
        printf("Error: Data size exceeds the maximum allowed limit per sector.\n");
        return;
    }

    // Ensure we don't exceed flash memory limits
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to write beyond flash memory limits.\n");
        return;
    }

    flash_data flashData;
    flashData.write_count = 0; 
    flashData.data_ptr = data; 
    flashData.data_len = data_len;
    flash_write_safe_struct(flash_offset, &flashData);
}

/// @brief  Writes data to flash memory at the specified offset.
/// @param offset  Offset in flash memory to write data to.
/// @param new_data     
void flash_write_safe_struct(uint32_t offset, flash_data *new_data) {
    
    
    
    uint32_t ints = save_and_disable_interrupts();

    // Temporary structure to hold the current data
    flash_data current_data;
    memset(&current_data, 0, METADATA_SIZE);

    // Read the existing data (if any)
    memcpy(&current_data, (const void *)(XIP_BASE + offset), METADATA_SIZE);
  
 
      // if no entry made yet, set write count to 0
    if(current_data.write_count == -1) {
        current_data.write_count = 0;
    } 

    // Increment the write count based on existing data
    new_data->write_count = current_data.write_count + 1;

    new_data->valid = true;
    // Erase the sector before writing new data
    flash_range_erase(offset, FLASH_SECTOR_SIZE);
    // Write the new data, including the updated write count
    flash_range_program(offset, (const uint8_t *)new_data, METADATA_SIZE);
    restore_interrupts(ints);
}


void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len) {
    
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    printf("flash_offset: %d\n", flash_offset);
     // Check if the offset is aligned with the sector size
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Invalid offset. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return;
    }


    flash_data flashData;
    memset(&flashData, 0, sizeof(flashData));  // Initialize to zero
    flash_read_safe_struct(flash_offset, &flashData);
    
    if (!flashData.valid || flashData.data_len == 0) {
        printf("Error: Attempt to read invalid or uninitialized flash data.\n");
        return;  // Add error handling or status return here
    }

    if (buffer_len > flashData.data_len) {
        buffer_len = flashData.data_len;
    }
    
    // Since data_ptr cannot point to a meaningful location, the data needs to be handled differently
    // Assume the actual data follows immediately after the metadata in flash
    memcpy(buffer, flashData.data_ptr, buffer_len);
    printf("Flash data is valid and read successfully.\n");
}

void flash_read_safe_struct(uint32_t offset, flash_data *data) {
    
    if (offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return;
    }

    // Reading Data
    memcpy(data, (const void *)(XIP_BASE + offset), METADATA_SIZE);
}

 





// Define the size of your metadata. Adjust this based on your actual metadata size.


void flash_erase_safe(uint32_t offset) {
    printf("offset: %d\n", offset);
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    printf("flash_offset: %d\n", flash_offset);
     // Check if the offset is aligned with the sector size
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Invalid offset. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return;
    }

    printf("flash_offset: %d\n", flash_offset);
    uint32_t ints = save_and_disable_interrupts();
    uint32_t sector_start = flash_offset & ~(FLASH_SECTOR_SIZE - 1);

    if (sector_start >= FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Sector start address is out of bounds.\n");
        restore_interrupts(ints);
        return;
    }
    // Backup only write_count and data_len
    flash_data metadata_backup;
    memcpy(&metadata_backup, (const void*)(XIP_BASE + sector_start), sizeof(flash_data));

    // Erase the sector
    flash_range_erase(sector_start, FLASH_SECTOR_SIZE);

 

    flash_data metadata_to_restore = {
    .valid = false,
    .write_count = metadata_backup.write_count,
    .data_len = 0,
    .data_ptr = NULL
    };
    
    printf("metadata_to_restore.valid: %d\n", metadata_to_restore.valid);
    // Restore metadata (write_count and data_len only)
    flash_range_program(sector_start, (const uint8_t *)&metadata_to_restore, sizeof(flash_data));

    restore_interrupts(ints);
}

  