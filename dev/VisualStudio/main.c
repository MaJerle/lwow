#include "stdafx.h"
#include "windows.h"

#include "ow/ow.h"
#include "ow/devices/ow_device_ds18x20.h"

ow_t ow;

/**
 * \brief           Application entry point
 */
int
main(void) {
    uint8_t ri[8], rom_addresses[20][8];
    size_t count, i;

    ow_init(&ow, NULL);                         /* Initialize 1-Wire module */

    /*
     * Scan for 1-wire devices
     */
    ow_protect(&ow);                            /* Protect against multiple-thread access */
    if (ow_search_reset(&ow) == owOK) {         /* Start search for devices */
        printf("Search for 1-Wire device started!\r\n");
        count = 0;
        while (ow_search(&ow, ri) == owOK) {    /* Scan device by device */
            memcpy(rom_addresses[count], ri, sizeof(ri));
            printf("Device found: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                ri[0], ri[1], ri[2], ri[3], ri[4], ri[5], ri[6], ri[7]
            );
            if (ow_ds18x20_is_b(&ow, rom_addresses[count])) {
                ow_ds18x20_set_resolution(&ow, rom_addresses[count], 9 + count % 4, 0);
            }
            if (count == 0) {
                ow_ds18x20_set_alarm_temp(&ow, ri, 26, 28, 0);
            } else {
                ow_ds18x20_set_alarm_temp(&ow, ri, OW_DS18X20_ALARM_DISABLE, OW_DS18X20_ALARM_DISABLE, 0);
            }
            count++;
        }
        printf("Search finished with %u devices on 1-Wire bus\r\n", (unsigned)count);
    } else {
        printf("Search reset was unsuccessful!\r\n");
    }
    ow_unprotect(&ow);                          /* We can now unprotect again */

    /*
     * Periodically start conversions
     */
    if (count > 0) {
        while (1) {
            printf("\r\n\r\nStart temperature conversion\r\n");
            ow_ds18x20_start(&ow, NULL, 1);     /* Start temperature conversion on all devices */
            Sleep(1000);                        /* Sleep for some time */
            printf("\r\n");
            for (i = 0; i < count; i++) {       /* Read all connected sensors */
                float temp;
                if (ow_ds18x20_is_b(&ow, rom_addresses[i])) {
                    uint8_t resolution = ow_ds18x20_get_resolution(&ow, rom_addresses[i], 1);
                    if (ow_ds18x20_read(&ow, rom_addresses[i], &temp, 1)) {
                        printf("Sensor %u returned temperature %f degrees (%u bits resolution)\r\n", (unsigned)i, temp, (unsigned)resolution);
                    }
                }
            }

            /* Search for devices with alarm flag */
            ow_search_reset(&ow);               /* Reset search */
            while (ow_ds18x20_search_alarm(&ow, ri) == owOK) {
                printf("Device with alarm: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    ri[0], ri[1], ri[2], ri[3], ri[4], ri[5], ri[6], ri[7]
                );
            }
        }
    } else {
        printf("No devices found on 1-Wire network\r\n");
    }
    return 0;
}

