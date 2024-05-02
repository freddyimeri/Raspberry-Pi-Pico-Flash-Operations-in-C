/**
 * @file flash_ops_helper.h
 * 
 * Header file defining operations for safe and reliable manipulation of flash memory on the Raspberry Pi Pico.
 * This file includes functions for reading, writing, and erasing data in flash memory while ensuring data integrity
 * through checks such as alignment, bounds, and data validity. The functions are designed to be used in environments
 * where direct control and robust handling of non-volatile memory are required.
 */

#ifndef FLASH_OPS_HELPER_H
#define FLASH_OPS_HELPER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>  // Include this header for bool type
#include "flash_ops.h"

// test structure
typedef struct {
    uint32_t id;
    float sensor_value;
    char name[10];
} DeviceConfig;



 
// Utility functions to get additional information from flash memory.
uint32_t get_flash_write_count(uint32_t offset); // Retrieves the write count for a specified offset.
uint32_t get_flash_data_length(uint32_t offset); // Retrieves the length of data stored at a specified offset.



void serialize_device_config(const DeviceConfig *config, uint8_t *buffer);
void deserialize_device_config(const uint8_t *buffer, DeviceConfig *config);


void serialize_flash_data(const flash_data *data, uint8_t *buffer, size_t buffer_size);
void deserialize_flash_data(const uint8_t *buffer, flash_data *data);


#endif // FLASH_OPS_H
