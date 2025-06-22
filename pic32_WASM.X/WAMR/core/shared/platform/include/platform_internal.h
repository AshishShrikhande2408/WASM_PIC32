/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _PLATFORM_INTERNAL_H
#define _PLATFORM_INTERNAL_H

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include "assert.h"
#include "limits.h"
#include "FreeRTOS.h"
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#include "task.h"

#ifndef WASM_ENABLE_LABELS_AS_VALUES
#define WASM_ENABLE_LABELS_AS_VALUES 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BH_PLATFORM_OPENRTOS
#define BH_PLATFORM_OPENRTOS
#endif
  
#ifndef __mips__
#define __mips__
#endif
    
struct mutex_temp
{
    SemaphoreHandle_t sem;
    bool is_recursive;  
};

typedef struct os_thread_wait_node 
{
    SemaphoreHandle_t sem;
     void* next;
} os_thread_wait_node;

typedef struct os_thread_wait_node *os_thread_wait_list;
struct cond_temp
{
    QueueHandle_t wait_list_lock;
    os_thread_wait_list thread_wait_list;
};
typedef struct mutex_temp korp_mutex;
typedef struct cond_temp korp_cond;
typedef TaskHandle_t korp_tid;

struct os_thread_wait_node;


typedef pthread_t korp_thread;
typedef pthread_rwlock_t korp_rwlock;
typedef unsigned int korp_sem;

/* Default thread priority */
#define BH_THREAD_DEFAULT_PRIORITY 5

/* On NuttX, time_t is uint32_t */
#define BH_TIME_T_MAX 0xffffffff

static inline int
os_getpagesize()
{
    return 4096;
}

typedef int os_file_handle;
typedef int *os_dir_stream;
typedef int os_raw_file_handle;

static inline os_file_handle
os_get_invalid_handle(void)
{
    return -1;
}

#ifdef __cplusplus
}
#endif

#endif
