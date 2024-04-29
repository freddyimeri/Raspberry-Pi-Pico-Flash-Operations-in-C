#include <stdio.h>
#include "pico/stdlib.h"

#include "flash_ops.h"

#include <stdlib.h>
#include <string.h>

int main() {
    stdio_init_all();
 
    // Wait for USB connection
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
        printf("USB Connected.\n");



  printf("Starting Flash Operations Test\n");

    // Data to be written to the flash
    const uint8_t write_data[] = "Hello, dynamic flash with pointers!";
    uint32_t offset = 1;  // Flash memory offset to start writing

    // Writing data to flash
    printf("Writing to flash...\n");
    flash_write_safe(offset, write_data, sizeof(write_data) - 1);  // Exclude null terminator
    flash_write_safe(offset, write_data, sizeof(write_data) - 1);  // Exclude null terminator
    flash_write_safe(offset, write_data, sizeof(write_data) - 1);  // Exclude null terminator
    flash_write_safe(offset, write_data, sizeof(write_data) - 1);  // Exclude null terminator
    // Buffer to read back data
    uint8_t read_buffer[256];  // Allocate a large enough buffer to hold read data
    memset(read_buffer, 0, sizeof(read_buffer));  // Clear the buffer

    // Reading data from flash
    printf("Reading from flash...\n");
    flash_read_safe(offset, read_buffer, sizeof(read_buffer));

    // Print the data read from flash
    printf("Data read from flash: %s\n", read_buffer);


    printf("\n\n\n\nPrinting data details...\n");


    print_flash_data_write_count(offset);

    printf("Data details printed.\n");


    // Demonstrate erasing flash
    printf("Erasing flash sector...\n");
    flash_erase_safe(offset);
    printf("Flash sector erased.\n");

    // Final message
    printf("Flash operations test completed.\n");
    return 0;
}

