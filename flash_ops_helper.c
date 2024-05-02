
#include "flash_ops.h"
#include "flash_ops_helper.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>
 #include <stdbool.h>  

#define FLASH_SIZE PICO_FLASH_SIZE_BYTES 
#define FLASH_TARGET_OFFSET (256 * 1024) 
#define METADATA_SIZE sizeof(flash_data)  

/**
 * Retrieves the write count for a specific sector in the flash memory. This function checks
 * that the given offset aligns with the flash sector size and is within the flash memory's
 * boundaries before accessing the flash data.
 *
 * @param offset The offset from the start of the flash memory for which to retrieve the write count.
 * @return The number of times the flash sector at the given offset has been written. Returns 0
 *         if there is an error due to misalignment or boundary overflow, or if the sector has not
 *         been written yet.
 */
uint32_t get_flash_write_count(uint32_t offset) {
    // Calculate the actual memory address within the flash memory where the write count is stored.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

    // Ensure that the offset is aligned with the flash sector size to prevent reading from an incorrect sector.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for getting the Write count. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return 0; // Return 0 as an error indicator due to misalignment.
    }

    // Check to ensure that the read operation stays within the bounds of the flash memory to avoid overflow errors.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read for write count beyond flash memory limits.\n");
        return 0; // Return 0 as an error indicator due to attempting to read beyond the flash memory limits.
    }

    // Define a temporary structure to store the data read from flash memory.
    flash_data tempFlashData;

    // Read the metadata from the specified offset within the flash memory.
    memcpy(&tempFlashData, (const void *)(XIP_BASE + flash_offset), METADATA_SIZE);

    // Return the retrieved write count. This count helps in understanding the wear level of the flash sector.
    return tempFlashData.write_count;
}



/**
 * Retrieves the data length for a specific sector in the flash memory. This function ensures
 * that the read operation respects alignment with the flash sector size and stays within the
 * memory boundaries, crucial for accurate data retrieval and system stability.
 *
 * @param offset The offset from the start of the flash memory from which to retrieve the data length.
 * @return The length of the data stored at the given offset, in bytes. Returns 0 if there is an
 *         error due to misalignment, boundary issues, or if the sector has not been initialized.
 */
uint32_t get_flash_data_length(uint32_t offset) {
    // Calculate the actual memory address in flash memory.
    uint32_t flash_offset = FLASH_TARGET_OFFSET + offset;

    // Ensure the offset is aligned with the flash sector size to avoid reading incomplete or incorrect data.
    if (flash_offset % FLASH_SECTOR_SIZE != 0) {
        printf("Error: Invalid offset for getting data length. Please use a multiple of %d (sector size).\n", FLASH_SECTOR_SIZE);
        return 0; // Return 0 to indicate an error due to misalignment.
    }

    // Check that the memory address for reading is within the allowed flash memory bounds.
    if (flash_offset + METADATA_SIZE > FLASH_TARGET_OFFSET + FLASH_SIZE) {
        printf("Error: Attempt to read for data length beyond flash memory limits.\n");
        return 0; // Return 0 to indicate an error due to reading beyond flash memory limits.
    }

    // Define a temporary structure to hold the flash data read from memory.
    flash_data tempFlashData;

    // Read the data from the specified flash memory offset.
    memcpy(&tempFlashData, (const void *)(XIP_BASE + flash_offset), METADATA_SIZE);

    // Output the data length for debugging and verification purposes.
    printf("FLASH DATA LENGTH: %zu\n", tempFlashData.data_len);

    // Return the data length retrieved from the flash memory.
    return tempFlashData.data_len;
}




void serialize_flash_data(const flash_data *data, uint8_t *buffer, size_t buffer_size) {
    // Ensure the buffer is large enough to hold all data including metadata
    size_t required_size = sizeof(data->valid) + sizeof(data->write_count) + sizeof(data->data_len) + data->data_len;
    if (buffer_size < required_size) {
        // Handle error: buffer not large enough
        printf("Buffer size is too small to serialize flash_data.\n");
        return;
    }

    // Serialize 'valid'
    memcpy(buffer, &data->valid, sizeof(data->valid));
    buffer += sizeof(data->valid);

    // Serialize 'write_count'
    memcpy(buffer, &data->write_count, sizeof(data->write_count));
    buffer += sizeof(data->write_count);

    // Serialize 'data_len'
    memcpy(buffer, &data->data_len, sizeof(data->data_len));
    buffer += sizeof(data->data_len);

    // Serialize actual data pointed by 'data_ptr'
    if (data->data_ptr != NULL && data->data_len > 0) {
        memcpy(buffer, data->data_ptr, data->data_len);
    }
    else {
        printf("Data pointer is NULL or data length is zero.\n");
    }
}




void deserialize_flash_data(const uint8_t *buffer, flash_data *data) {
    printf("entering deserialize_flash_data\n");
    memcpy(&data->valid, buffer, sizeof(data->valid));
    buffer += sizeof(data->valid);
    memcpy(&data->write_count, buffer, sizeof(data->write_count));
    buffer += sizeof(data->write_count);
    memcpy(&data->data_len, buffer, sizeof(data->data_len));
    buffer += sizeof(data->data_len);

    data->data_ptr = malloc(data->data_len);
    if (data->data_ptr == NULL) {
        printf("Failed to allocate memory for data_ptr.\n");
        return;
    }
    memcpy(data->data_ptr, buffer, data->data_len);
}





// Function to deserialize a byte array back into DeviceConfig
void deserialize_device_config(const uint8_t *buffer, DeviceConfig *config) {
    memcpy(&config->id, buffer, sizeof(config->id));
    buffer += sizeof(config->id);
    memcpy(&config->sensor_value, buffer, sizeof(config->sensor_value));
    buffer += sizeof(config->sensor_value);
    memcpy(config->name, buffer, sizeof(config->name));
}
 


 


void serialize_device_config(const DeviceConfig *config, uint8_t *buffer) {
    memcpy(buffer, &config->id, sizeof(config->id));
    buffer += sizeof(config->id);
    memcpy(buffer, &config->sensor_value, sizeof(config->sensor_value));
    buffer += sizeof(config->sensor_value);
    memcpy(buffer, config->name, sizeof(config->name));
}




uint8_t* prepare_buffer(const char* text, size_t *buffer_size) {
    size_t text_len = strlen(text) + 1; // Include null terminator
    *buffer_size = ((text_len + 255) / 256) * 256; // Align to next multiple of 256 bytes

    uint8_t* buffer = calloc(1, *buffer_size);
    if (buffer) {
        memcpy(buffer, text, text_len); // Copy the text into the buffer
    }
    return buffer;
}

void verify_data(const uint8_t* original, const uint8_t* read_back, size_t size) {
    if (memcmp(original, read_back, size) == 0) {
        printf("Data verified successfully: %s\n", read_back);
    } else {
        printf("Data verification failed!\n");
    }
}