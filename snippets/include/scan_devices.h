#ifndef __SCAN_DEVICES_H
#define __SCAN_DEVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ow/ow.h"

owr_t	scan_onewire_devices(ow_t* ow, uint8_t rom_ids[][8], size_t rtf, size_t* rf);

#ifdef __cplusplus
}
#endif

#endif
