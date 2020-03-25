#ifndef __SCAN_DEVICES_H
#define __SCAN_DEVICES_H

#include "ow/ow.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

owr_t	scan_onewire_devices(ow_t* ow, ow_rom_t* rom_ids, size_t rtf, size_t* rf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
