/**
 * @file flash_ops.h
 * 
 * Header file defining operations for the safe and reliable manipulation of flash memory on the Raspberry Pi Pico.
 * This interface provides essential functions for reading, writing, and erasing data in flash memory,
 * ensuring data integrity through rigorous checks on alignment, boundary limits, and data validity.
 * These functions are crucial in environments requiring direct control and robust management of non-volatile memory.
 */

#ifndef FLASH_OPS_H
#define FLASH_OPS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>  

/**
 * A structure to encapsulate flash data along with metadata for enhanced reliability and management.
 * This structure is used to track and verify the integrity of stored data.
 */
typedef struct {
    bool valid;             // Indicates if the data is considered valid.
    uint32_t write_count;   // Tracks the number of times the data has been written to ensure wear leveling.
    size_t data_len;        // Specifies the length of the data in bytes.
    uint8_t *data_ptr;      // Points to the actual data stored in flash.
} flash_data;

// Functions for manipulating flash memory
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len); // Writes data to flash safely.
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len); // Reads data from flash safely.
void flash_erase_safe(uint32_t offset); // Erases a sector of flash memory safely.

 
#endif // FLASH_OPS_H
