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

    lwow_init(&ow, &lwow_ll_drv_win32, NULL);   /* Initialize 1-Wire library and set user argument to 1 */

    /* Get onewire devices connected on 1-wire port */
    while (1) {
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

                lwow_ds18x20_start(&ow, NULL);  /* Start conversion on all devices */
                Sleep(1500);                    /* Release thread for 1 second */

                /* Read temperature on all devices */
                lwow_protect(&ow, 1);
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
                lwow_unprotect(&ow, 1);
            }
        }
    }
    printf("Terminating application thread\r\n");
    return 0;
}

