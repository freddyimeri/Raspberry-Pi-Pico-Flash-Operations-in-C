#include "flash_ops.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts (256KB into flash)
#define FLASH_SIZE PICO_FLASH_SIZE_BYTES // Total flash size available

// Function: flash_write_safe
// Writes data to flash memory at a specified offset, ensuring safety checks.
//
// Parameters:
// - offset: The offset from FLASH_TARGET_OFFSET where data is to be written.
// - data: Pointer to the data to be written.
// - data_len: Length of the data to be written.
//
// Note: This function erases the flash sector before writing new data.

void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len) {
 
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

     // Bounds checking
    if (flash_offset + data_len > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to write beyond flash memory limits.\n");
        return; // Exit if the operation is out of bounds
    }
    
    uint32_t ints = save_and_disable_interrupts();

    flash_range_erase(flash_offset, FLASH_SECTOR_SIZE);

    flash_range_program(flash_offset, data, data_len);

    restore_interrupts(ints);

}

// Function: flash_read_safe
// Reads data from flash memory into a buffer.
//
// Parameters:
// - offset: The offset from FLASH_TARGET_OFFSET where data is to be read.
// - buffer: Pointer to the buffer where read data will be stored.
// - buffer_len: Number of bytes to read.
//
// Note: The function performs bounds checking to ensure safe access.

void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len) {

uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

// Bounds checking

 if (flash_offset + buffer_len > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read beyond flash memory limits.\n");
        return; // Exit if the operation is out of bounds
    }

memcpy(buffer, (const void *)(XIP_BASE + flash_offset), buffer_len);

} 

void flash_erase_safe(uint32_t offset) {
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    uint32_t ints = save_and_disable_interrupts();
    uint32_t sector_start = flash_offset & ~(FLASH_SECTOR_SIZE - 1);
    if (sector_start < FLASH_TARGET_OFFSET + FLASH_SIZE) {
    	flash_range_erase(sector_start, FLASH_SECTOR_SIZE);
    } 
    else 
    {
        printf("Error: Sector start address is out of bounds.\n");
        // Restore interrupts before returning due to error
        restore_interrupts(ints);
        return;
    }
    restore_interrupts(ints);
}














