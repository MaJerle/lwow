#include "windows.h"
#include <stdio.h>

#include "lwow/lwow.h"
#include "lwow/devices/lwow_device_ds18x20.h"
#include "scan_devices.h"

/* Create new 1-Wire instance */
extern const lwow_ll_drv_t lwow_ll_drv_win32;
lwow_t ow;
lwow_rom_t rom_ids[20];
size_t rom_found;

/**
 * \brief           Application entry point
 */
int
main(void) {
    printf("Starting OneWire application..\r\n");

    /* Initialize 1-Wire library and set user argument to 1 */
    if (lwow_init(&ow, &lwow_ll_drv_win32, NULL) != lwowOK) {
        printf("Could not initialize LwOW..\r\n");
        while (1) { Sleep(1000); }
    }

    /* Get onewire devices connected on 1-wire port */
    while (1) {
        /* 
         * Scan for devices connected on 1-wire port
         *
         * This is a pre-defined function from snippets part of the lib
         */
        printf("Scanning 1-Wire port...\r\n");
        if (scan_onewire_devices(&ow, rom_ids, LWOW_ARRAYSIZE(rom_ids), &rom_found) == lwowOK) {
            printf("Devices scanned, found %d device%s!\r\n", (int)rom_found, "s" + (rom_found == 1));
        } else {
            printf("Device scan error\r\n");
            Sleep(1000);
        }

        if (rom_found > 0) {
            /* Infinite loop */
            for (size_t c = 0; c < 5; c++) {
                printf("Start temperature conversion\r\n");

                /*
                 * Enable OW protection, to prevent access
                 * from other threads to the OW core
                 */
                lwow_protect(&ow, 1);

                /* Start temperature conversion on all DS18x20 devices */
                lwow_ds18x20_start_raw(&ow, NULL);

                /*
                 * Sleep for some time
                 * 
                 * Keep OW core protected to make sure
                 * other threads won't reset the bus and reset measurement being in process
                 */
                Sleep(2000);

                /* Read temperature on all devices */
                for (size_t i = 0; i < rom_found; ++i) {
                    if (lwow_ds18x20_is_b(&ow, &rom_ids[i])) {
                        float temp;
                        uint8_t resolution = lwow_ds18x20_get_resolution_raw(&ow, &rom_ids[i]);
                        if (lwow_ds18x20_read_raw(&ow, &rom_ids[i], &temp)) {
                            printf("Sensor %3u temperature is %d.%03d degrees (%u bits resolution)\r\n",
                                (unsigned)i, (int)temp, (int)((temp * 1000.0f) - (((int)temp) * 1000)), (unsigned)resolution);
                        }
                    }
                }

                /*
                 * Sensors are now idle, data has been read well
                 *
                 * Release access to allow other threads to get access
                 */
                lwow_unprotect(&ow, 1);
            }
        }
    }
    printf("Terminating application thread\r\n");
    return 0;
}

