/**
 * \file            onewire.h
 * \brief           OneWire protocol
 */
#ifndef __ONEWIRE_H__
#define __ONEWIRE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum {
    owOK = 0x00,                                /*!< Device returned OK */
    owERRPRESENCE = -1,                         /*!< Presence was not successful */
    owERRNODEV = -2,                            /*!< No device connected, maybe device removed during scan? */
    owERR,                                      /*!< General-Purpose error */
} ow_res_t;

/**
 * \brief           1-Wire structure
 */
typedef struct {    
#if OW_USE_RTOS || __DOXYGEN__
    osMutexId mutex;                            /*!< Mutex handle */
#endif /* OW_USE_RTOS || __DOXYGEN__ */
    
    uint8_t rom[8];                             /*!< ROM address of last device found.
                                                     When searching for new devices, we always need last found address,
                                                     to be able to decide which way to go next time during scan. */
    uint8_t disrepancy;                         /*!< Disrepancy value on last search */

} ow_t;

ow_res_t    ow_init(ow_t* ow);

ow_res_t    ow_lock(ow_t* ow);
ow_res_t    ow_unlock(ow_t* ow);

ow_res_t    ow_reset(ow_t* ow);
uint8_t     ow_write_byte(ow_t* ow, uint8_t byte);
uint8_t     ow_read_byte(ow_t* ow);

ow_res_t    ow_search_reset(ow_t* ow);
ow_res_t    ow_search(ow_t* ow, uint8_t *id);

#ifdef __cplusplus
}
#endif

#endif /* __ONEWIRE_H__ */
