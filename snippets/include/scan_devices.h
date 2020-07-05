#ifndef SNIPPET_HDR_SCAN_DEVICES_H
#define SNIPPET_HDR_SCAN_DEVICES_H

#include "lwow/lwow.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

lwowr_t   scan_onewire_devices(lwow_t* ow, lwow_rom_t* rom_ids, size_t rtf, size_t* rf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SNIPPET_HDR_SCAN_DEVICES_H */
