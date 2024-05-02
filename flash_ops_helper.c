
/**
 * @file flash_ops_helper.c
 *
 * This file contains a collection of helper functions designed to support operations on the flash memory
 * of the Raspberry Pi Pico. It extends the basic functionalities defined in flash_ops.h by providing
 * more specific, often complex, operations that involve reading and writing structured data to and from
 * the flash memory, as well as managing serialization and deserialization processes.
 *
 * Detailed functionalities included:
 * - Reading and writing structured data to ensure proper alignment and data integrity.
 * - Serializing and deserializing data to/from byte streams, which is crucial for storing complex data structures.
 * - Handling metadata associated with flash operations to support robust and reliable memory access.
 * - Performing memory and data integrity checks to ensure that operations on the flash memory are safe and effective.
 * - Utilities like buffer preparation and data verification to assist in data management and debugging.
 *
 * These utilities are essential for applications that require direct and low-level manipulation of flash memory,
 * ensuring that data is handled efficiently and safely while adhering to the technical specifications and
 * limitations of the hardware.
 */


#include "flash_ops.h"
#include "flash_ops_helper.h"
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdlib.h>

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




/**
 * Serializes the flash_data structure into a buffer for writing to flash memory.
 * This process converts the structured data into a continuous byte stream that can be stored easily.
 *
 * @param data Pointer to the flash_data structure to be serialized.
 * @param buffer Buffer where serialized data will be stored.
 * @param buffer_size Size of the buffer provided for serialization.
 */
void serialize_flash_data(const flash_data *data, uint8_t *buffer, size_t buffer_size) {
    // Calculate the total size required for the serialized data including all metadata and actual data.
    size_t required_size = sizeof(data->valid) + sizeof(data->write_count) + sizeof(data->data_len) + data->data_len;

    // Check if the provided buffer is large enough to hold the serialized data.
    if (buffer_size < required_size) {
        printf("Buffer size is too small to serialize flash_data. Required: %zu, Given: %zu\n", required_size, buffer_size);
        return;  // Exit the function if the buffer does not have sufficient space to avoid buffer overflow.
    }

    // Serialize the 'valid' field - this indicates whether the data is considered valid.
    memcpy(buffer, &data->valid, sizeof(data->valid));
    buffer += sizeof(data->valid);  // Move the buffer pointer forward by the size of the 'valid' field.

    // Serialize the 'write_count' field - this tracks how many times the data has been written.
    memcpy(buffer, &data->write_count, sizeof(data->write_count));
    buffer += sizeof(data->write_count);  // Move the buffer pointer forward by the size of the 'write_count' field.

    // Serialize the 'data_len' field - this specifies the length of the data.
    memcpy(buffer, &data->data_len, sizeof(data->data_len));
    buffer += sizeof(data->data_len);  // Move the buffer pointer forward by the size of the 'data_len' field.

    // Serialize the actual data pointed by 'data_ptr', if it exists and has a non-zero length.
    if (data->data_ptr != NULL && data->data_len > 0) {
        memcpy(buffer, data->data_ptr, data->data_len);  // Copy the actual data into the buffer.
    } else {
        printf("Data pointer is NULL or data length is zero, nothing to serialize for actual data.\n");
    }
}





/**
 * Deserializes data from a byte buffer into a flash_data structure, reconstructing the original structured data.
 * This function is typically used after reading serialized data from flash memory.
 *
 * @param buffer Pointer to the buffer containing serialized flash data.
 * @param data Pointer to the flash_data structure where the deserialized data will be stored.
 */
void deserialize_flash_data(const uint8_t *buffer, flash_data *data) {
    // Announce the start of the deserialization process for debugging purposes.
    printf("Entering deserialize_flash_data\n");

    // Copy the 'valid' field from the buffer to the structure. This field indicates data validity.
    memcpy(&data->valid, buffer, sizeof(data->valid));
    buffer += sizeof(data->valid);  // Advance the buffer pointer to the next piece of data.

    // Copy the 'write_count' field, which tracks the number of times the data has been written.
    memcpy(&data->write_count, buffer, sizeof(data->write_count));
    buffer += sizeof(data->write_count);  // Advance the buffer pointer.

    // Copy the 'data_len' field, indicating the length of the data.
    memcpy(&data->data_len, buffer, sizeof(data->data_len));
    buffer += sizeof(data->data_len);  // Move the buffer pointer to the start of the actual data.

    // Allocate memory for the data pointed by 'data_ptr' based on the length provided in 'data_len'.
    data->data_ptr = malloc(data->data_len);
    if (data->data_ptr == NULL) {
        printf("Failed to allocate memory for data_ptr.\n");
        return;  // Exit if memory allocation fails, to prevent further errors.
    }

    // Copy the actual data into the newly allocated memory.
    memcpy(data->data_ptr, buffer, data->data_len);
}






/**
 * Deserializes a byte array back into a DeviceConfig structure. This function reconstructs the 
 * DeviceConfig structure from a serialized state in a buffer, typically after reading from storage
 * or receiving over a network.
 *
 * @param buffer Pointer to the buffer containing the serialized DeviceConfig data.
 * @param config Pointer to the DeviceConfig structure where the deserialized data will be stored.
 */
void deserialize_device_config(const uint8_t *buffer, DeviceConfig *config) {
    // Copy the device ID from the buffer into the structure. This is typically a unique identifier.
    memcpy(&config->id, buffer, sizeof(config->id));
    buffer += sizeof(config->id);  // Move the buffer pointer past the ID field.

    // Copy the sensor value, which may represent a measurement or status from a device sensor.
    memcpy(&config->sensor_value, buffer, sizeof(config->sensor_value));
    buffer += sizeof(config->sensor_value);  // Move the buffer pointer past the sensor value field.

    // Copy the device name, which is stored as a fixed-length string.
    memcpy(config->name, buffer, sizeof(config->name));
    // The buffer pointer is moved past the name field automatically by the size of the name.
}



 


/**
 * Serializes a DeviceConfig structure into a byte array. This function converts the structured data
 * from a DeviceConfig structure into a linear byte sequence for easy storage or transmission.
 *
 * @param config Pointer to the DeviceConfig structure containing the data to be serialized.
 * @param buffer Pointer to the buffer where the serialized data will be stored.
 */
void serialize_device_config(const DeviceConfig *config, uint8_t *buffer) {
    // Copy the device ID into the buffer. This is usually a numeric identifier.
    memcpy(buffer, &config->id, sizeof(config->id));
    buffer += sizeof(config->id);  // Advance the buffer pointer to the next write position.

    // Copy the sensor value, which could represent any data measured by the device's sensors.
    memcpy(buffer, &config->sensor_value, sizeof(config->sensor_value));
    buffer += sizeof(config->sensor_value);  // Advance the buffer pointer.

    // Copy the device name, a character array, into the buffer.
    memcpy(buffer, config->name, sizeof(config->name));
    // No need to move the buffer pointer after this as it's typically the last item in the structure.
}





/**
 * Prepares a buffer to hold a given text string, ensuring the buffer is sized to a multiple of 256 bytes.
 * This function calculates the required buffer size to include the null terminator and then aligns this size
 * up to the nearest multiple of 256 bytes. This alignment can be useful for certain types of memory operations
 * that are optimized for specific alignment.
 *
 * @param text Pointer to the null-terminated string that needs to be copied into the buffer.
 * @param buffer_size Pointer to a size_t variable where the function will store the size of the allocated buffer.
 * @return Pointer to the allocated buffer containing the copied text or NULL if memory allocation fails.
 */
uint8_t* prepare_buffer(const char* text, size_t *buffer_size) {
    // Calculate the length of the input text including the null terminator.
    size_t text_len = strlen(text) + 1;  // +1 for null terminator

    // Round up the required size to the next multiple of 256 bytes.
    *buffer_size = ((text_len + 255) / 256) * 256; // Ensures alignment for memory operations

    // Allocate memory for the buffer using calloc to initialize memory to zero.
    uint8_t* buffer = calloc(1, *buffer_size);
    if (buffer) {
        // If allocation was successful, copy the text into the buffer.
        memcpy(buffer, text, text_len);  // Copy includes the null terminator
    }
    // Return the pointer to the newly allocated buffer, or NULL if allocation failed.
    return buffer;
}


 
/**
 * Verifies that two data buffers are identical by comparing their content byte-by-byte.
 * This function is useful for ensuring data integrity, particularly after data transmission
 * or storage operations, where it confirms that data has not been altered or corrupted.
 *
 * @param original Pointer to the buffer containing the original data.
 * @param read_back Pointer to the buffer containing the data to compare against the original.
 * @param size The number of bytes to compare in both data buffers.
 */
void verify_data(const uint8_t* original, const uint8_t* read_back, size_t size) {
    // Compare the original data buffer to the read-back data buffer.
    if (memcmp(original, read_back, size) == 0) {
        // If the content of the buffers is identical, print a success message.
        // Note: Using `%s` to print `read_back` assumes it is null-terminated and contains text data.
        // If `read_back` may contain non-text data or may not be null-terminated, this could lead to undefined behavior.
        printf("Data verified successfully: %s\n", read_back);
    } else {
        // If the buffers differ, print a failure message indicating verification failed.
        printf("Data verification failed!\n");
    }
}
