#include <stdio.h>
#include "scan_devices.h"
#include "ow/ow.h"

/**
 * \brief           Scan for 1-Wire devices on specific 1-Wire port
 * \param[in]       ow: 1-Wire handle
 * \param[in]       rom_ids: pointer to array of \ref ow_rom_t structures
 * \param[in]       rtf: Number of roms to find
 * \param[out]      rf: Number of roms found after scan
 */
owr_t
scan_onewire_devices(ow_t* ow, ow_rom_t* rom_ids, size_t rtf, size_t* rf) {
    owr_t res;
    size_t found;
    
    /* Search for devices and save it to array */
    res = ow_search_devices(ow, rom_ids, rtf, &found);

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
