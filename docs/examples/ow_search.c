ow_t ow;
owr_t res;
uint8_t rom_id[8];

ow_init(&ow, NULL);

ow_protect(&ow, 1);
res = ow_search_reset(&ow, 0);
/* Search until all devices scanned */
while (res == owOK && (res = ow_search(&ow, rom_id, 0)) == owOK) {
	printf("New device found!\r\n");
    printf("ID: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
        rom_id[0], rom_id[1], rom_id[2], rom_id[3],
        rom_id[4], rom_id[5], rom_id[6], rom_id[7],
    );
}
ow_unprotect(&ow, 1);
