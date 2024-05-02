#include "cli.h"
#include "flash_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "custom_fgets.h"

void execute_command(char *command) {
    char *token = strtok(command, " ");
    if (token == NULL) {
        printf("\nInvalid command\n");
        return;
    }

    if (strcmp(token, "FLASH_WRITE") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL) {
            printf("\nFLASH_WRITE requires an address and data\n");
            return;
        }
        uint32_t address = atoi(token);

        token = strtok(NULL, "\"");
        if (token == NULL) {
            printf("\nInvalid data format for FLASH_WRITE\n");
            return;
        }

        // Prepare the structure for writing
        flash_data data_to_write;
        memset(&data_to_write, 0, sizeof(data_to_write)); // Ensure the structure is clear
        // memcpy(data_to_write.data, token, strlen(token)); // Copy the string into the structure's data field
        data_to_write.data_len = strlen(token); // Set the data length

        // Call the modified write function
        //flash_write_safe_struct(address, &data_to_write);
    }
    else if (strcmp(token, "FLASH_READ") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL) {
            printf("\nFLASH_READ requires an address\n");
            return;
        }
        uint32_t address = atoi(token);

        // Prepare a structure to receive the read data
        flash_data data_read;
        memset(&data_read, 0, sizeof(data_read)); // Clear the structure

        // Call the modified read function
        //flash_read_safe_struct(address, &data_read);


        // Displaying the metadata
        printf("\nMetadata: Write count = %u\n", data_read.write_count);

        // Print the read data
        printf("\nData: ");
        for (size_t i = 0; i < data_read.data_len; i++) {
            // printf("%c", data_read.data[i]);
        }
        printf("\n");
    }
    else if (strcmp(token, "FLASH_ERASE") == 0) {
        token = strtok(NULL, " ");
        if (token == NULL) {
            printf("\nFLASH_ERASE requires an address\n");
            return;
        }
        uint32_t address = atoi(token);

        // Call the existing erase function
        flash_erase_safe(address);
    }
    else {
        printf("\nUnknown command\n");
    }
}
