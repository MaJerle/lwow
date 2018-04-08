/**
 * \file            usart.h
 * \brief           OneWire USART application
 */
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdint.h"
#include "stddef.h"

void      ow_usart_tr(const void* tx, void* rx, size_t len);
void      ow_usart_set_baud(uint32_t baud);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
