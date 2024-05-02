# Table of Contents

1. [Introduction](#introduction)
   - [Project Overview](#project-overview)
   - [Key Features](#key-features)
2. [Getting Started](#getting-started)
   - [Prerequisites](#prerequisites)
   - [Installation](#installation)
   - [Building the Project](#building-the-project)
3. [Usage](#usage)
   - [Basic Examples](#basic-examples)
   - [Advanced Features](#advanced-features)
4. [Architecture](#architecture)
   - [System Overview](#system-overview)
   - [Memory Management](#memory-management)
5. [Function Details](#function-details)
   - [flash_write_safe](#flash_write_safe)
   - [flash_read_safe](#flash_read_safe)
   - [flash_erase_safe](#flash_erase_safe)
6. [Structured Data Management](#structured-data-management)
   - [flash_data Structure](#flash_data-structure)
   - [Metadata Management](#metadata-management)
7. [Testing](#testing)
   - [Test Suite Overview](#test-suite-overview)
   - [Running Tests](#running-tests)
8. [UML Diagrams](#uml-diagrams)
   - [Function Interaction](#function-interaction)
   - [Data Flow](#data-flow)
9. [Contributing](#contributing)
10. [License](#license)
11. [Contact](#contact)






## Function Details: `flash_write_safe`

### Overview
The `flash_write_safe` function is designed to write data to the flash memory in a safe and controlled manner. It ensures that all write operations adhere to flash memory constraints such as alignment requirements and sector boundaries, which are critical for avoiding data corruption.

### Signature
```c
void flash_write_safe(uint32_t offset, const uint8_t *data, size_t data_len);
```
### Parameters

- **`offset`**: The offset from the base of the flash memory where data writing begins. This offset needs to be aligned with the flash sector size.
- **`data`**: Pointer to the data buffer that needs to be written to flash.
- **`data_len`**: The length of the data in bytes that will be written to flash.

### Operational Logic

- **Offset Validation**:
  - The function first checks if the offset is aligned with the flash sector size. Flash memory typically requires that write operations begin at sector boundaries to avoid partial writes that can lead to data corruption.

- **Data Validation**:
  - It checks if the data pointer is NULL and if `data_len` is greater than zero. Writing operations with invalid data pointers or zero length are rejected to prevent runtime errors and undefined behavior.

- **Boundary Checks**:
  - The function calculates if the data to be written stays within the physical memory limits of the flash. This prevents any attempt to write data beyond the available flash memory, which can overwrite other critical data or system areas.

- **Sector Erase and Write**:
  - Before new data can be programmed into flash memory, the relevant sector(s) must be erased. `flash_write_safe` handles this by first erasing the sector at the specified offset.
  - After erasing, the data is written to the flash memory. This two-step process ensures that the flash memory is ready to receive new data and that the data is written correctly.

### Importance

- **Data Integrity**: Ensures that each write operation is performed correctly without corrupting other data, crucial for systems where data reliability is key.
- **System Stability**: By enforcing boundary and alignment checks, the function prevents system crashes and data corruption scenarios that could arise from improper flash memory operations.
- **Performance Considerations**: While ensuring safety, the function is also designed to be as efficient as possible under the constraints of operating directly with hardware.

### Example Usage

Here's how you might use `flash_write_safe` in an application:

```c
#include "flash_ops.h"

void example_write_to_flash() {
    const uint8_t sample_data[] = {0x01, 0x02, 0x03, 0x04};
    uint32_t flash_offset = 0x1000; // Example offset, aligned as required.

    flash_write_safe(flash_offset, sample_data, sizeof(sample_data));
    // Additional logic to verify the write or handle errors could be added here.
}
```

## Capabilities and Limitations of `flash_write_safe`

| Feature                                           | Supported          |
|---------------------------------------------------|--------------------|
| Data Alignment Verification                       | :white_check_mark: |
| Boundary Check                                    | :white_check_mark: |
| Sector Erase Before Writing                       | :white_check_mark: |
| Data Size Validation                              | :white_check_mark: |
| Handling of Null Data Pointers                    | :white_check_mark: |
| Increment Write Counter                           | :white_check_mark: |
| Serialization of Data Structures                  | :white_check_mark: |
| Writing Beyond Flash Memory Limits                | :x:                |
| Automatic Wear Leveling                           | :x:                |
| Real-Time Data Encryption                         | :x:                |
| Asynchronous Writing                              | :x:                |
| Data Compression Before Writing                   | :x:                |

### Detailed Feature Descriptions

- **Data Alignment Verification**: Ensures that write operations begin at sector boundaries to avoid partial writes and potential data corruption.
- **Boundary Check**: Verifies that write operations do not exceed the physical limits of the flash memory, safeguarding against overwriting critical data.
- **Sector Erase Before Writing**: Automatically erases the sector before programming new data, ensuring a clean state for reliable data storage.
- **Data Size Validation**: Checks if the data length exceeds the permissible limits for a single sector, accounting for metadata, and rejects oversized data.
- **Handling of Null Data Pointers**: Properly manages cases with null data pointers to prevent runtime errors.
- **Increment Write Counter**: Accurately tracks and increments the write count for each sector, which is crucial for monitoring flash wear and implementing wear leveling strategies.
- **Serialization of Data Structures**: Effectively serializes structured data into a byte array for flash storage, handling complex data types efficiently.
- **Writing Beyond Flash Memory Limits**: Does not support operations that attempt to write data beyond the established flash memory boundaries, to prevent system instability and data loss.
- **Automatic Wear Leveling**: The function does not automatically distribute write operations to extend the lifespan of the flash memory, which could be a limitation in systems requiring enhanced durability.
- **Real-Time Data Encryption**: Lacks the capability to encrypt data on-the-fly before writing to flash, which might be necessary for applications with high security requirements.
- **Asynchronous Writing**: Operates synchronously, which might not be optimal for applications requiring non-blocking operations.
- **Data Compression Before Writing**: Does not compress data before writing, which could be a limitation in scenarios where memory utilization efficiency is critical.


### Conclusion

This function is a foundational component of flash memory management in embedded applications, especially in systems like the Raspberry Pi Pico where direct control over hardware is a frequent requirement. It showcases how embedded systems handle low-level operations that are typically abstracted away in higher-level programming environments. This detailed control and precise handling ensure that the hardware's capabilities are maximally utilized while maintaining system integrity and reliability.```

This passage gives a concluding overview of the significance of the `flash_write_safe` function in the context of embedded system programming, emphasizing its critical role in low-level hardware management. It could be included towards the end of the section discussing the `flash_write_safe` function to wrap up the explanation and highlight the importance of such functions in practical scenarios.




<img src="images/Flash_write_safe.png" alt="Image Alt Text" width="600" height="600">



## Function Details: `flash_read_safe`

### Overview

The `flash_read_safe` function is designed to safely read data from the flash memory at a specified offset, ensuring that the operation adheres to flash memory alignment and boundary requirements to prevent data corruption and access errors. This function only proceeds with reading if the data is marked valid and is correctly initialized.

### Signature

```c
void flash_read_safe(uint32_t offset, uint8_t *buffer, size_t buffer_len);

```
## Parameters

- **`offset`**: The offset from the base of the flash memory where the read starts.
- **`buffer`**: The buffer to store the read data.
- **`buffer_len`**: The length of the buffer to ensure no overflow occurs during the read.

## Operational Logic

- **Offset Calculation**:
  - Calculates the actual flash memory address by adding the provided offset to the base address of the flash. This address is used for all subsequent read operations.

- **Offset Validation**:
  - Checks if the flash offset is aligned with the sector size. Misalignment can cause partial sector read issues, leading to potential data corruption.

- **Boundary Check**:
  - Verifies that the read operation does not extend beyond the flash memory's physical limits, protecting against attempts to access undefined memory areas.

- **Memory Allocation**:
  - Allocates a buffer large enough to hold the metadata and the requested data. This buffer is used to temporarily store data read from flash.

- **Data Transfer**:
  - Copies data from the flash memory starting at the calculated offset into the allocated buffer. This includes the metadata and the actual data.

- **Data Validation and Copy**:
  - Deserializes the buffer into a structured format to access the metadata and validates the integrity of the data.
  - If the data is marked as valid and the buffer size is sufficient, it copies the data to the user-provided buffer. Otherwise, it handles errors such as invalid data or insufficient buffer size.

- **Cleanup**:
  - Frees the allocated buffer to avoid memory leaks.

## Error Handling

Handles several error scenarios gracefully, providing clear error messages for:
- **Misalignment of the read offset**.
- **Attempts to read beyond the flash memory's boundaries**.
- **Insufficient buffer size provided by the caller**.
- **Encountering invalid data at the specified offset**.

## Example Usage

Here's how you might use `flash_read_safe` in an application:

```c
#include "flash_ops.h"

void example_read_from_flash() {
    uint8_t buffer[256]; // Allocate a buffer for reading
    uint32_t offset = 0x1000; // Example offset, aligned as required

    flash_read_safe(offset, buffer, sizeof(buffer));
    // Additional logic to verify the read or handle errors could be added here
}
```

## Capabilities and Limitations of `flash_read_safe`

| Feature                                           | Supported          | Details                                                      |
|---------------------------------------------------|--------------------|--------------------------------------------------------------|
| Offset Alignment Verification                     | :white_check_mark: | Ensures read operations begin at sector boundaries.          |
| Boundary Check                                    | :white_check_mark: | Prevents reading beyond the flash memory's physical limits.  |
| Handling of NULL Pointers for Buffer              | :x:                | Does not explicitly handle NULL pointers for buffer.         |
| Data Validity Check                               | :white_check_mark: | Proceeds only if data is marked valid.                       |
| Memory Allocation for Read Operation              | :white_check_mark: | Allocates memory dynamically to handle read data.            |
| Error Handling for Misalignment and Overflows     | :white_check_mark: | Provides clear error messages for misalignment and overflows. |
| Deserialization of Data                           | :white_check_mark: | Converts data from raw flash format to structured data.      |
| Buffer Size Validation                            | :white_check_mark: | Checks if provided buffer is sufficient for data length.     |
| Real-Time Data Decompression                      | :x:                | Does not support decompression of data.                      |
| Data Encryption/Decryption Handling               | :x:                | Does not handle encrypted data.                              |
| Asynchronous Read Operations                      | :x:                | Supports only synchronous operations.                        |
| Automatic Memory Management                       | :x:                | Requires manual management of memory allocation/free.        |

### Additional Notes
- **Offset Alignment Verification** and **Boundary Check** are crucial for maintaining the integrity of data reads, ensuring that operations do not disrupt the system's stability by reading inappropriate memory areas.
- **Data Validity Check** ensures that corrupted or uninitialized data is not mistakenly read, which could lead to errors or undefined behavior in the application using this data.
- **Error Handling**: The function is robust in providing feedback for various common errors such as misalignment or attempts to read beyond the available memory, which helps in debugging and maintaining the system.
- **Memory Management**: Users of this function need to be aware that while the function allocates memory for reading data, it also requires the caller to manage the memory post-operation, specifically to free the allocated buffer.

This table aims to provide a comprehensive overview of what the `flash_read_safe` function can do and its limitations, helping developers understand how to integrate it into their systems and what additional measures might be needed to handle its limitations.




<img src="images/read_safe1.png" alt="Image Alt Text" width="600" height="600">



## Function Details: `flash_erase_safe`

### Overview

The `flash_erase_safe` function is meticulously designed to securely erase a sector of the flash memory at a specified offset. This critical operation adheres strictly to alignment and boundary conditions specific to flash memory. By ensuring that the sector is correctly aligned and within the permissible boundaries, the function avoids partial erasures and potential data corruption. During the erase process, it preserves essential metadata, which is crucial for maintaining data integrity and supporting overall system stability. This function is pivotal in applications where data security and integrity are paramount.

### Signature

```c
void flash_erase_safe(uint32_t offset);
```
## Parameters

- **`offset`**: The offset within the flash memory where the sector begins to be erased.

## Operational Logic

- **Offset Calculation and Validation**:
  - The function calculates the actual address in flash memory where the erasure will start, factoring in the specified offset.
  - It checks if this offset aligns with the sector size. Misalignment can lead to partial sector erasures, which might cause data integrity issues.

- **Boundary Check**:
  - Ensures that the erasing process does not exceed the limits of the flash memory to prevent corrupting data outside the intended sector.

- **Write Count Retrieval and Increment**:
  - Retrieves the current write count for the sector from metadata, increments it to track the number of erase cycles, aiding in wear leveling.

- **Interrupts Handling**:
  - Disables interrupts to maintain the atomicity of the erase operation, ensuring it is not interrupted and potentially left incomplete.

- **Sector Start Calculation**:
  - Computes the exact start of the sector ensuring it aligns correctly with sector boundaries.

- **Metadata Backup and Erase**:
  - Backs up metadata before erasing to preserve critical data like write counts.
  - Performs the actual erasure of the sector.

- **Metadata Restoration**:
  - After erasing, restores essential metadata at the start of the sector to maintain data integrity. Marks data as invalid since the sector has been erased.

- **Restore Operation State**:
  - Re-enables interrupts after completing the erase to return the system to its normal operational state.

## Error Handling

- **Offset Misalignment**: Returns an error if the offset does not align with sector boundaries, preventing accidental data corruption.
- **Boundary Exceedance**: Stops the operation with an error if the erasure would go beyond flash memory limits, safeguarding against potential system instability.

## Example Usage

Here's a basic example of how to use `flash_erase_safe` in an application:

```c
#include "flash_ops.h"

void example_erase_sector() {
    uint32_t offset = 0x1000; // Ensure this offset is within the flash memory's limits and properly aligned
    flash_erase_safe(offset);
    // Additional logic to handle post-erase operations or check results might be added here
}
```


## Capabilities and Limitations of `flash_erase_safe`

| Feature                                       | Supported          | Details                                                      |
|-----------------------------------------------|--------------------|--------------------------------------------------------------|
| Alignment Verification                        | :white_check_mark: | Ensures erasure only occurs if the offset aligns with sector boundaries. |
| Boundary Check                                | :white_check_mark: | Prevents erasure beyond the flash memory's physical limits.  |
| Metadata Preservation                         | :white_check_mark: | Preserves essential metadata like write counts before erasing. |
| Error Handling for Misalignment and Overflows | :white_check_mark: | Provides clear error messages for misalignment and attempts to erase beyond limits. |
| Interrupt Handling                            | :white_check_mark: | Disables interrupts during erasure to ensure operation atomicity. |
| Sector Erasure Accuracy                       | :white_check_mark: | Computes exact sector start and performs precise erasure.    |
| Metadata Restoration                          | :white_check_mark: | Restores metadata post-erasure to ensure flash management data integrity. |
| Automatic Reallocation                        | :x:                | Does not handle reallocation of data within flash storage.   |
| Asynchronous Operation                        | :x:                | Operation is synchronous, potentially blocking system operations during erase. |
| Data Recovery                                 | :x:                | Does not support recovery of erased data, ensuring data is irrecoverable post-erase. |

### Additional Notes
- **Alignment Verification** and **Boundary Check** are critical to avoid data corruption and ensure that the erase operation affects only the intended sector.
- **Error Handling**: The function is robust in providing feedback and preventing operations that could lead to system instability or data loss.
- **Interrupt Handling** and **Sector Erasure Accuracy** are essential for maintaining system performance and integrity during potentially disruptive operations like erasing flash sectors.
- **Metadata Restoration** helps in maintaining the management and operational continuity of flash memory by ensuring that metadata remains consistent and accurate, even after an erase operation.
- **Limitations**: The synchronous nature of the operation could impact system responsiveness, especially in systems with real-time requirements.

This table should be useful for developers and system architects in understanding the operational scope, reliability features, and potential constraints of the `flash_erase_safe` function.






<img src="images/erase.png" alt="Image Alt Text" width="600" height="600">
