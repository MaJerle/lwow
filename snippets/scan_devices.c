#include <stdio.h>
#include "scan_devices.h"
#include "lwow/lwow.h"

/**
 * \brief           Scan for 1-Wire devices on specific 1-Wire port
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_ids: pointer to array of \ref lwow_rom_t structures
 * \param[in]       rtf: Number of roms to find
 * \param[out]      rf: Number of roms found after scan
 */
lwowr_t
scan_onewire_devices(lwow_t* ow, lwow_rom_t* rom_ids, size_t rtf, size_t* rf) {
    lwowr_t res;
    size_t found;

    /* Search for devices and save it to array */
    res = lwow_search_devices(ow, rom_ids, rtf, &found);

    /* Print all devices */
    for (size_t i = 0; i < found; ++i) {
        printf("Device ROM addr: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
            (unsigned)rom_ids[i].rom[0], (unsigned)rom_ids[i].rom[1], (unsigned)rom_ids[i].rom[2], (unsigned)rom_ids[i].rom[3],
            (unsigned)rom_ids[i].rom[4], (unsigned)rom_ids[i].rom[5], (unsigned)rom_ids[i].rom[6], (unsigned)rom_ids[i].rom[7]
        );
    }
    if (rf != NULL) {
    	*rf = found;
    }
    return res;
}
