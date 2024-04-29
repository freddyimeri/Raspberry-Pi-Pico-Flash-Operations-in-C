#ifndef FLASH_OPS_H
#define FLASH_OPS_H

#include <stdint.h>
#include <stddef.h>

 
typedef struct {
    uint32_t write_count;
    size_t data_len;
    uint8_t *data_ptr;
} flash_data;


// Function for manipulating to flash memory
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len);
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len);
void flash_erase_safe(uint32_t offset);

// Helper functions
void flash_write_safe_struct(uint32_t offset, flash_data *data);
void flash_read_safe_struct(uint32_t offset, flash_data *data);


 
#endif // FLASH_OPS_H



