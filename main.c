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



    return 0;
}

