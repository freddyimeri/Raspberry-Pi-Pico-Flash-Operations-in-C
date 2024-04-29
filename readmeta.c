// #include "flash_ops.h"
// #include <stdio.h>
// #include <string.h>
 
// #include "pico/stdlib.h"
// #include "hardware/flash.h"
// #include "hardware/sync.h"
// #include <stdlib.h>

// #define FLASH_TARGET_OFFSET (256 * 1024) // Offset where user data starts (256KB into flash)
// #define FLASH_SIZE PICO_FLASH_SIZE_BYTES // Total flash size available

 

// void flash_read_safe_struct(uint32_t offset, flash_data *data) {
//     printf("\nENTERED flash_read_safe_struct\n ");
//     uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;
//     printf("flash_offset: %d\n", flash_offset); 
//     // Bounds checking
//     if (flash_offset + sizeof(flash_data) > FLASH_TARGET_OFFSET + FLASH_SIZE) {
//         printf("Error: Attempt to read beyond flash memory limits.\n");
//         return;
//     }

//     // Reading Data
//     memcpy(data, (const void *)(XIP_BASE + flash_offset), sizeof(flash_data));
// }





// // Adapter for flash_read_safe to work with cli.c without modifications
// void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len) {
//     flash_data flashData;
//     flash_read_safe_struct(offset, &flashData);

//     if (buffer_len > flashData.data_len) buffer_len = flashData.data_len;
//     printf("FLASH DATA: %s\n", flashData.data_ptr);
//     printf("FLASH DATA LEN: %d\n", flashData.data_len); 
//     printf("FLASH DATA WRITE COUNT: %d\n", flashData.write_count);
//     memcpy(buffer, flashData.data_ptr, buffer_len);
// }


// uint32_t flash_get_write_count(uint32_t offset) {
//     flash_data flashData;
//     printf("\nENTERED flash_get_write_count\n ");
//     printf("OFFSET: %d\n", offset);
//     flash_read_safe_struct(offset, &flashData);
//     return flashData.write_count;
// }
 