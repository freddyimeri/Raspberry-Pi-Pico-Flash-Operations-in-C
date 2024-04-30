/**
 * @file flash_ops.h
 * 
 * Header file defining operations for safe and reliable manipulation of flash memory on the Raspberry Pi Pico.
 * This file includes functions for reading, writing, and erasing data in flash memory while ensuring data integrity
 * through checks such as alignment, bounds, and data validity. The functions are designed to be used in environments
 * where direct control and robust handling of non-volatile memory are required.
 */

#ifndef FLASH_OPS_H
#define FLASH_OPS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>  // Include this header for bool type

// Structure to represent the flash data with metadata for reliability and tracking.
typedef struct {
    bool valid;              // Flag to indicate if the data is valid
    uint32_t write_count;    // Number of times the data has been written
    size_t data_len;         // Length of the data in bytes
    const uint8_t *data_ptr; // Pointer to the actual data
} flash_data;

// Functions for manipulating flash memory
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len); // Writes data to flash safely.
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len); // Reads data from flash safely.
void flash_erase_safe(uint32_t offset); // Erases a sector of flash memory safely.

// Helper functions to support the main operations with structured access to flash.
void flash_write_safe_struct(uint32_t offset, flash_data *data); // Writes structured data to flash.
void flash_read_safe_struct(uint32_t offset, flash_data *data); // Reads structured data from flash.

// Utility functions to get additional information from flash memory.
uint32_t get_flash_write_count(uint32_t offset); // Retrieves the write count for a specified offset.
uint32_t get_flash_data_length(uint32_t offset); // Retrieves the length of data stored at a specified offset.

#endif // FLASH_OPS_H
