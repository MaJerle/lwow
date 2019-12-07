ow_t ow;
owr_t res;
ow_rom_t rom_id;

ow_init(&ow, NULL);

ow_protect(&ow, 1);
res = ow_search_reset_raw(&ow, 0);
/* Search until all devices scanned */
while (res == owOK && (res = ow_search_raw(&ow, &rom_id, 0)) == owOK) {
	printf("New device found!\r\n");
    printf("ID: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
        (unsigned)rom_id.rom[0], (unsigned)rom_id.rom[1], (unsigned)rom_id.rom[2], (unsigned)rom_id.rom[3],
        (unsigned)rom_id.rom[4], (unsigned)rom_id.rom[5], (unsigned)rom_id.rom[6], (unsigned)rom_id.rom[7]
    );
}
ow_unprotect(&ow, 1);
