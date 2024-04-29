#include "flash_ops.h"
#include <stdio.h>
#include <string.h>
 
#include "pico/stdlib.h"
#include "hardware/flash.h"




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




void flash_erase_safe(uint32_t offset) {
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
    uint32_t ints = save_and_disable_interrupts();
    uint32_t sector_start = flash_offset & ~(FLASH_SECTOR_SIZE - 1);

    // Assuming metadata is at the start of each sector and has a fixed size
    const size_t metadata_size = sizeof(flash_data);  // Change size as per your metadata structure

    if (sector_start < FLASH_TARGET_OFFSET + FLASH_SIZE) {
        // Backup metadata before erasing
        uint8_t metadata_backup[metadata_size];
        memcpy(metadata_backup, (const void *)(XIP_BASE + sector_start), metadata_size);

        // Erase the sector
        flash_range_erase(sector_start, FLASH_SECTOR_SIZE);

        // Restore metadata
        flash_range_program(sector_start, metadata_backup, metadata_size);
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
