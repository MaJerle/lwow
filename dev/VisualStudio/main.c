#include "stdafx.h"
#include "windows.h"

#include "ow/ow.h"
#include "ow/devices/ow_device_ds18x20.h"
#include "scan_devices.h"

/* Create new 1-Wire instance */
ow_t ow;
uint8_t rom_ids[20][8];
size_t rom_found;

/**
 * \brief           Application entry point
 */
int
main(void) {
    size_t i;

    ow_init(&ow, NULL);                         /* Initialize 1-Wire library and set user argument to 1 */

    /* Get onewire devices connected on 1-wire port */
    if (scan_onewire_devices(&ow, rom_ids, sizeof(rom_ids) / sizeof(rom_ids[0]), &rom_found) == owOK) {
        printf("Devices scanned, found %d devices!\r\n", (int)rom_found);
    } else {
    	printf("Device scan error\r\n");
    }

    if (rom_found) {
        /* Infinite loop */
        while (1) {
            printf("Start temperature conversion\r\n");
            ow_protect(&ow, 1);
            ow_ds18x20_start(&ow, NULL);        /* Start conversion on all devices */
            ow_unprotect(&ow, 1);
            Sleep(1000);                        /* Release thread for 1 second */

            /* Read temperature on all devices */
            ow_protect(&ow, 1);
            for (i = 0; i < rom_found; i++) {
                if (ow_ds18x20_is_b(&ow, rom_ids[i])) {
                    float temp;
                    uint8_t resolution = ow_ds18x20_get_resolution(&ow, rom_ids[i]);
                    if (ow_ds18x20_read(&ow, rom_ids[i], &temp)) {
                        ow_unprotect(&ow, 1);
                        printf("Sensor %u temperature is %d.%d degrees (%u bits resolution)\r\n",
                            (unsigned)i, (int)temp, (int)((temp * 1000.0f) - (((int)temp) * 1000)), (unsigned)resolution);
                        ow_protect(&ow, 1);
                    }
                }
            }
            ow_unprotect(&ow, 1);
        }
    }
    printf("Terminating application thread\r\n");
    return 0;
}

