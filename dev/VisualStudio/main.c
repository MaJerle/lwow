#include "windows.h"
#include "stdio.h"

#include "ow/ow.h"
#include "ow/devices/ow_device_ds18x20.h"
#include "scan_devices.h"

/* Create new 1-Wire instance */
ow_t ow;
ow_rom_t rom_ids[20];
size_t rom_found;

/**
 * \brief           Application entry point
 */
int
main(void) {
    printf("Starting OneWire application..\r\n");

    ow_init(&ow, NULL);                         /* Initialize 1-Wire library and set user argument to 1 */

    /* Get onewire devices connected on 1-wire port */
scan:
    printf("Scanning 1-Wire port...\r\n");
    if (scan_onewire_devices(&ow, rom_ids, OW_ARRAYSIZE(rom_ids), &rom_found) == owOK) {
        printf("Devices scanned, found %d device%s!\r\n", (int)rom_found, "s" + (rom_found == 1));
    } else {
        printf("Device scan error\r\n");
        Sleep(1000);
    }

    if (rom_found > 0) {
        /* Infinite loop */
        for (size_t c = 0; c < 5; c++) {
            printf("Start temperature conversion\r\n");

            ow_ds18x20_start(&ow, NULL);        /* Start conversion on all devices */
            Sleep(1500);                        /* Release thread for 1 second */

            /* Read temperature on all devices */
            ow_protect(&ow, 1);
            for (size_t i = 0; i < rom_found; i++) {
                if (ow_ds18x20_is_b(&ow, &rom_ids[i])) {
                    float temp;
                    uint8_t resolution = ow_ds18x20_get_resolution_raw(&ow, &rom_ids[i]);
                    if (ow_ds18x20_read_raw(&ow, &rom_ids[i], &temp)) {
                        printf("Sensor %u temperature is %d.%d degrees (%u bits resolution)\r\n",
                            (unsigned)i, (int)temp, (int)((temp * 1000.0f) - (((int)temp) * 1000)), (unsigned)resolution);
                    }
                }
            }
            ow_unprotect(&ow, 1);
        }
    }
    goto scan;
    printf("Terminating application thread\r\n");
    return 0;
}

