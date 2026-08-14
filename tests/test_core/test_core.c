#include <stdio.h>
#include <string.h>
#include "lwow/lwow.h"
#include "test.h"

static uint8_t
ll_init(void* arg) {
    LWOW_UNUSED(arg);
    return 1;
}

static uint8_t
ll_deinit(void* arg) {
    LWOW_UNUSED(arg);
    return 1;
}

static uint8_t
ll_set_baudrate(uint32_t baud, void* arg) {
    LWOW_UNUSED(baud);
    LWOW_UNUSED(arg);
    return 1;
}

static uint8_t
ll_tx_rx(const uint8_t* tx, uint8_t* rx, size_t len, void* arg) {
    LWOW_UNUSED(arg);
    memmove(rx, tx, len);
    return 1;
}

/* Loopback low-level driver, no real 1-Wire hardware required */
static const lwow_ll_drv_t ll_drv_stub = {
    .init = ll_init,
    .deinit = ll_deinit,
    .set_baudrate = ll_set_baudrate,
    .tx_rx = ll_tx_rx,
};

int
test_run(void) {
    lwow_t ow;
    static const uint8_t crc_input[7] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    static const uint8_t crc_full[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x0F};

    printf("LwOW core test\r\n");

    /* CRC-8/Maxim must match a known-good vector and self-check to 0 with its own CRC appended */
    TEST_ASSERT(lwow_crc(crc_input, sizeof(crc_input)) == 0x0F);
    TEST_ASSERT(lwow_crc(crc_full, sizeof(crc_full)) == 0);

    /* Init/protect/unprotect/deinit must all succeed through the selected OS mutex port */
    TEST_ASSERT(lwow_init(&ow, &ll_drv_stub, NULL) == lwowOK);
    TEST_ASSERT(lwow_protect(&ow, 1) == lwowOK);
    TEST_ASSERT(lwow_unprotect(&ow, 1) == lwowOK);
    lwow_deinit(&ow);

    printf("LwOW core test passed\r\n");
    return 0;
}
