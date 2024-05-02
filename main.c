#include <stdio.h>
#include "pico/stdlib.h"
#include "test.h"

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
    printf("Running all tests...\n");
    run_all_tests();

    printf("buyeeeeeeee all tests...\n");
    return 0;
}

