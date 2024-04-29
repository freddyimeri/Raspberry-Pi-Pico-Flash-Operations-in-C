#include "flash_ops.h"
#include <stdio.h>
#include <string.h>
 
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>

#define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts (256KB into flash)
#define FLASH_SIZE PICO_FLASH_SIZE_BYTES // Total flash size available

void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len) {
    printf("\nENTERED flash_write_safe\n ");

    flash_data flashData;
    flashData.write_count = 0; 
    flashData.data_ptr = data; 
    flashData.data_len = data_len;
    flash_write_safe_struct(offset, &flashData);
}

/// @brief  Writes data to flash memory at the specified offset.
/// @param offset  Offset in flash memory to write data to.
/// @param new_data     
void flash_write_safe_struct(uint32_t offset, flash_data *new_data) {
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    // Ensure we don't exceed flash memory limits
    if (flash_offset + sizeof(flash_data) > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to write beyond flash memory limits.\n");
        return;
    }
    
    uint32_t ints = save_and_disable_interrupts();

    // Temporary structure to hold the current data
    flash_data current_data;
    memset(&current_data, 0, sizeof(flash_data));

    // Read the existing data (if any)
    memcpy(&current_data, (const void *)(XIP_BASE + flash_offset), sizeof(flash_data));
  
 
      // if no entry made yet, set write count to 0
    if(current_data.write_count == -1) {
        current_data.write_count = 0;
    } 

    // Increment the write count based on existing data
    new_data->write_count = current_data.write_count + 1;
    // Erase the sector before writing new data
    flash_range_erase(flash_offset, FLASH_SECTOR_SIZE);
    // Write the new data, including the updated write count
    flash_range_program(flash_offset, (const uint8_t *)new_data, sizeof(flash_data));
    restore_interrupts(ints);
}


void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len) {
    flash_data flashData;
    flash_read_safe_struct(offset, &flashData);
    if (buffer_len > flashData.data_len) buffer_len = flashData.data_len;
    memcpy(buffer, flashData.data_ptr, buffer_len);
}

void flash_read_safe_struct(uint32_t offset, flash_data *data) {
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
     // Bounds checking
    if (flash_offset + sizeof(flash_data) > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return;
    }

    // Reading Data
    memcpy(data, (const void *)(XIP_BASE + flash_offset), sizeof(flash_data));
}




// Define the size of your metadata. Adjust this based on your actual metadata size.
#define METADATA_SIZE sizeof(flash_data)  

void flash_erase_safe(uint32_t offset) {
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
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

    // Prepare the metadata to be written back without the original data pointer
    flash_data metadata_to_restore = {
        metadata_backup.write_count,
        metadata_backup.data_len,
        NULL // Reset data_ptr or set it to a valid memory if needed
    };

    // Restore metadata (write_count and data_len only)
    flash_range_program(sector_start, (const uint8_t *)&metadata_to_restore, sizeof(flash_data));

    restore_interrupts(ints);
}



 void print_flash_data_write_count(uint32_t offset) {
    printf("\nENTERED print_flash_data_write_count\n");
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

    // Bounds checking
    if (flash_offset + sizeof(flash_data) > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return;
    }

    // Temporary structure to hold flash data
    flash_data tempFlashData;

    // Reading Data
    memcpy(&tempFlashData, (const void *)(XIP_BASE + flash_offset), sizeof(flash_data));

    // Print the write count
    printf("FLASH DATA WRITE COUNT: %d\n", tempFlashData.write_count);
}