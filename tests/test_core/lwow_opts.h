/**
 * \file            lwow_opts.h
 * \brief           OW application options
 */
#ifndef LWOW_HDR_OPTS_H
#define LWOW_HDR_OPTS_H

#define LWOW_CFG_OS 1

#if defined(_WIN32)
#include <windows.h>
#define LWOW_CFG_OS_MUTEX_HANDLE HANDLE
#else
#include <pthread.h>
#define LWOW_CFG_OS_MUTEX_HANDLE pthread_mutex_t*
#endif

#endif /* LWOW_HDR_OPTS_H */
