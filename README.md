# Table of Contents

1. [Introduction](#introduction)
    - [Project Overview](#project-overview)
    - [Purpose and Significance](#purpose-and-significance)

2. [Features and Capabilities](#features-and-capabilities)
    - [What the Project Does](#what-the-project-does)
    - [Key Functionalities](#key-functionalities)

3. [Limitations](#limitations)
    - [Known Constraints and Limitations](#known-constraints-and-limitations)

4. [Getting Started](#getting-started)
    - [Installation](#installation)
    - [Setup Instructions](#setup-instructions)

5. [Usage](#usage)
    - [How to Use the Software](#how-to-use-the-software)
    - [Code Examples](#code-examples)

6. [Architecture and Design](#architecture-and-design)
    - [System Architecture Diagram](#system-architecture-diagram)
    - [Component Interaction](#component-interaction)

7. [Testing](#testing)
    - [How to Run Tests](#how-to-run-tests)
    - [Test Descriptions](#test-descriptions)



8. [Authors and Acknowledgment](#authors-and-acknowledgment)
    - [Credits](#credits)
    - [Acknowledgments](#acknowledgments)


9. [Contact](#contact)
    - [Developer Contact Information](#developer-contact-information)

 



## Introduction

### Project Overview

This project focuses on managing flash memory operations on the Raspberry Pi Pico, a popular microcontroller in the embedded systems field. The provided codebase offers functionalities for writing, reading, and erasing data in the non-volatile flash memory of the Raspberry Pi Pico. These operations are essential for applications requiring reliable data persistence even after power-down.

### Purpose

The primary purpose of this project is to implement and demonstrate effective methods for flash memory management on the Raspberry Pi Pico. Efficiently managing flash memory is critical for extending the device's memory lifespan and ensuring data integrity across power cycles. This project serves as a foundational toolset for developers aiming to utilize the Pico's hardware capabilities in various applications, from simple data logging to complex IoT solutions.

### Significance

The significance of this project lies in its ability to provide robust tools for persistent data storage, addressing common challenges in embedded systems development. The Raspberry Pi Pico, like many microcontrollers, operates in environments where power interruptions are frequent, and data loss can be costly. By leveraging the Pico's onboard flash memory, this project helps safeguard critical data against interruptions, ensuring safe and efficient data retention.

Incorporating best practices in flash memory operations not only enhances the reliability of embedded systems but also improves their performance and scalability. This project contributes to the broader Raspberry Pi Pico community by offering a tested, deployable set of functions that manage flash memory with precision and care.


## Features and Capabilities

This project provides a comprehensive suite of tools for managing flash memory on the Raspberry Pi Pico, enabling robust and reliable non-volatile data storage. Here are the key features and capabilities that distinguish our project:

### Flash Write Safe

- **Secure Data Writing:** Utilizes optimized algorithms to safely write data to the flash memory. Ensures data integrity by preventing the overwriting of critical system areas, including the bootloader and currently running applications.
- **Boundary Checking:** Automatically checks that data writes do not exceed flash memory limits, preventing potential data corruption and system crashes.

### Flash Read Safe

- **Efficient Data Retrieval:** Implements efficient methods to read data back from flash memory, ensuring fast and reliable access to stored information.
- **Error Handling:** Includes robust error handling to deal with scenarios where requested data may not be available or the read operations are out of the designated memory bounds.

### Flash Erase Safe

- **Sector Erase:** Provides functionality to erase data at specified offsets, allowing for the reuse of memory sectors safely and effectively.
- **Safe Operations:** Ensures that erase operations are performed safely without impacting neighboring data sectors, crucial for maintaining data integrity.

### Structured Data Handling

- **Metadata Management:** Each data block written includes metadata, such as write counts and data length, which aids in managing the lifecycle of each flash memory sector.
- **Enhanced Data Integrity:** Uses structured data blocks to encapsulate and manage user data more effectively, reducing errors and enhancing the performance of memory operations.

### Comprehensive Testing

- **Robust Test Suite:** Includes a detailed set of tests that cover all functionalities, from basic operations to edge cases, ensuring that every aspect of the software behaves as expected under various conditions.
- **Error Simulation:** Simulates common flash memory errors to verify that the system can handle unexpected scenarios gracefully.

### Architecture Flexibility

- **Modular Design:** Crafted with a modular approach, allowing developers to easily integrate these operations into larger projects or modify them to suit specific requirements.
- **Documentation and Examples:** Extensive documentation and example usage scenarios are provided, making it easy for new users to implement and adapt the functionalities in their projects.




## Limitations

While the flash memory management tools provided in this project offer robust functionalities for the Raspberry Pi Pico, there are several limitations that users should be aware of. Understanding these limitations will help users better integrate and utilize these tools within their specific contexts.

### Hardware Specificity

- **Platform Dependency:** The current implementation is specifically tailored for the Raspberry Pi Pico. As such, direct application to other microcontrollers or Raspberry Pi models may require modifications to accommodate different hardware architectures, especially concerning flash memory specifications and access methods.

### Memory Wear

- **Write Wear:** Flash memory has a limited number of write cycles. Excessive writing to the flash can lead to wear-out, which might not be optimally managed by the current version of the software. Users should implement wear-leveling techniques to extend the lifespan of their flash memory, which are not currently provided by this project.

### Performance Constraints

- **Memory Access Speeds:** The performance of flash memory operations (write, read, erase) is constrained by the hardware capabilities of the Raspberry Pi Pico. Intensive operations may lead to slower system performance, particularly noticeable in applications requiring high-speed data logging or frequent memory access.

### Error Handling

- **Limited Recovery Options:** While the project includes basic error handling, more complex error recovery mechanisms are not implemented. This could be a limitation in critical systems where data recovery capabilities are essential.

### Scalability

- **Fixed Memory Utilization:** The code assumes a fixed flash memory layout and size, which may not scale well with future versions of hardware or different configurations. Adjusting the code to dynamically handle varying memory sizes or layouts would require additional development.

### Integration Complexity

- **Integration Effort:** Integrating these tools into existing projects or systems may require a significant understanding of both the project's architecture and the target system’s memory management protocols.

### Documentation and Support

- **Community Resources:** While documentation is provided, the project currently lacks a broad community support base or forums for troubleshooting specific issues. This might limit users' ability to solve unique problems or adapt functionalities to custom applications.


## Getting Started

These instructions will guide you through the setup process to install and run the flash memory management tools on your Raspberry Pi Pico. Follow these steps carefully to ensure a smooth setup and successful operation.

### Prerequisites

Before you begin, make sure you have the following:

- A Raspberry Pi Pico microcontroller.
- Access to a computer with development tools installed (e.g., C/C++ compiler, CMake).
- The Raspberry Pi Pico SDK installed. Follow the Raspberry Pi Pico SDK setup guide if you haven't done this yet.

### Installation

#### Clone the Repository

Clone this repository to your local machine using the following command:

```bash
git clone https://github.com/yourusername/yourrepositoryname.git
```

Navigate to the cloned directory:

```bash
cd yourrepositoryname
```

Build the Project

Create a build directory

```bash
mkdir build
cd build
```

Configure the project with CMake:

```bash
cmake ..
```

Compile the code:

```bash
make
```

This will generate the executable and any necessary libraries.

### Running the Code

To run the program, connect your Raspberry Pi Pico to your computer and upload the compiled code using the USB connection. Detailed instructions on uploading code to the Raspberry Pi Pico can be found in the official Raspberry Pi Pico documentation.

### First Use

After successfully uploading the code, you can interact with the flash memory using the implemented functions:

- **Writing data to flash:** Call `flash_write_safe()` with the desired offset, data pointer, and data length.
- **Reading data from flash:** Use `flash_read_safe()` to retrieve data from a specified flash address.
- **Erasing flash sectors:** Use `flash_erase_safe()` to clear data from specified sectors.

### Troubleshooting

If you encounter issues during installation or execution, ensure all dependencies are correctly installed, and the SDK setup is complete. Review the error messages for clues and check the connections to your Raspberry Pi Pico.




## Usage

This section explains how to use the flash memory management functions implemented in the project. Here, you'll find examples on how to write, read, and erase data in the flash memory of the Raspberry Pi Pico, along with detailed descriptions of each function's parameters and expected behaviors.

### Writing Data to Flash

To write data to the flash memory, use the `flash_write_safe()` function. This function ensures that the data is written safely to the specified location without corrupting other parts of the flash memory.

#### Example:

```c
#include "flash_ops.h"

// Data to be written
const uint8_t data[] = "Hello, flash memory!";
uint32_t offset = 0x1000;  // Start writing at this flash memory offset

// Writing data to flash
flash_write_safe(offset, data, sizeof(data));

```

Parameters:

- offset: The offset in flash memory where data will start being written.
- data: Pointer to the data array to be written.
- data_len: Length of the data array.

### Reading Data from Flash

To read data from the flash memory, use the `flash_read_safe()` function. This function reads data from a specified flash memory location into a buffer.

#### Example:

```c
#include "flash_ops.h"

uint8_t buffer[100];  // Buffer to store the read data
uint32_t offset = 0x1000;  // Read from this flash memory offset

// Reading data from flash
flash_read_safe(offset, buffer, sizeof(buffer));


```

Parameters:

- offset: The offset in flash memory from where to start reading.
- buffer: Buffer to store the read data.
- buffer_len: Size of the buffer.

### Erasing Data from Flash

To erase data in the flash memory, use the `flash_erase_safe()` function. This function erases the data in specified sectors, making them ready for new data.

#### Example:

```c
#include "flash_ops.h"

uint32_t offset = 0x1000;  // Start erasing at this flash memory offset

// Erasing flash sector
flash_erase_safe(offset);

```
Parameters:

- offset: The offset in flash memory from where the erase should start.

### Additional Functionality

For advanced users needing to handle more complex scenarios or integrate additional functionalities, refer to the detailed API documentation provided in the project files. This includes handling metadata, optimizing performance, and error management.


## Architecture and Design

This section outlines the architecture and design of the flash memory management system developed for the Raspberry Pi Pico. The system is designed with modularity and efficiency in mind, focusing on the robust management of flash memory operations—write, read, and erase.

### System Components

The project is structured into several key components:

- **Flash Operations Module:** This module contains all the logic required to interact with the flash memory hardware. It encapsulates the `flash_write_safe`, `flash_read_safe`, and `flash_erase_safe` functions.
- **Data Management Module:** Manages data integrity and formats data blocks for writing. It also parses blocks when reading from flash.
- **Error Handling Module:** Provides robust error detection and handling strategies to ensure the system's reliability and stability.
- **Test Module:** Contains comprehensive tests that validate each function of the flash operations module under various conditions.

### Interaction Diagram

The following diagram illustrates how these components interact within the system:

DIAGRAMMMMMMMMMMMMMMMMMMMMM

## Design Principles

- **Modularity:** Each component is designed to be independent yet inter-operable. This allows for easier maintenance and future enhancements without significant disruptions to existing functionality.
- **Robustness:** Emphasis on error handling and validations ensures that the system can recover gracefully from common failures, enhancing its reliability.
- **Efficiency:** Optimized algorithms for flash memory operations reduce wear and enhance performance, crucial for embedded systems with limited resources.

### Hardware Interface

The system directly interacts with the Raspberry Pi Pico's hardware flash memory. It leverages the low-level APIs provided by the Pico SDK to perform physical memory operations while ensuring data safety and integrity.



## Testing

The project includes a comprehensive suite of tests to ensure the robust functionality and reliability of the flash memory management system on the Raspberry Pi Pico. These tests are designed to verify each aspect of flash memory operations, including writing, reading, and erasing under various conditions.

### Test Implementation

Tests are integrated within the `main.c` file, which allows for direct execution on the Raspberry Pi Pico hardware. The `run_all_tests()` function is called here, which sequentially executes all defined test functions.

### Running the Tests

To run the tests, you will need to compile and upload the code to your Raspberry Pi Pico with the `main.c` file as the entry point. Here is how you can execute the tests:

1. **Compile and Upload:**
   Follow the setup instructions provided in the Getting Started section to compile the project and upload it to your Raspberry Pi Pico.

2. **Execute the Tests:**
   Once the Pico is powered and connected via USB, the tests will automatically start upon execution of the program. Ensure that your terminal or serial monitor is open to view the test results.

   Here is what the relevant part of your `main.c` looks like:


```c
int main() {
    stdio_init_all();

    // Wait for USB connection
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("USB Connected.\n");

    // Running tests
    printf("Running all tests...\n");
    run_all_tests();

    // Completion message
    printf("Byeeeeee all tests...\n");

    return 0;
}

```


## Understanding Test Outputs

Each test function will output its results to the console, indicating whether it has passed or failed along with any relevant messages. This immediate feedback will help you understand the behavior of the system under test.

## Troubleshooting Test Failures

If tests fail:

- Review the console messages for specific errors or clues.
- Ensure that your Raspberry Pi Pico is correctly connected and functioning.
- Revisit the setup instructions to verify that the environment is configured correctly.
- Check for hardware issues that might affect the test outcomes, such as improper connections or faulty components.

## Authors and Acknowledgment

The original template for this project was cloned from [https://gitlab.uwe.ac.uk/f4-barnes/cap_template](https://gitlab.uwe.ac.uk/f4-barnes/cap_template) by Frazer Barnes.
