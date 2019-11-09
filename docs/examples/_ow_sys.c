/* Config header */
#define OW_CFG_OS_MUTEX_HANDLE      osMutexId

/* System file */
uint8_t
ow_sys_mutex_create(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    osMutexDef(m);
    *mutex = osMutexCreate(osMutex(m));         /* Create new mutex */
    return 1;
}

uint8_t
ow_sys_mutex_delete(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    osMutexDelete(*mutex);                      /* Delete mutex */
    return 1;
}

uint8_t
ow_sys_mutex_wait(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    if (osMutexWait(*mutex, osWaitForever) != osOK) {
        return 0;
    }
    return 1;
}

uint8_t
ow_sys_mutex_release(OW_CFG_OS_MUTEX_HANDLE* mutex, void* arg) {
    if (osMutexRelease(*mutex) != osOK) {
        return 0;
    }
    return 1;
}