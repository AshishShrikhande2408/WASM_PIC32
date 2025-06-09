/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include "platform_api_vmcore.h"
#include "platform_api_extension.h"

int
bh_platform_init()
{
    return 0;
}

void
bh_platform_destroy()
{}

int
os_printf(const char *format, ...)
{
    int ret = 0;
    va_list ap;

    va_start(ap, format);
#ifndef BH_VPRINTF
    ret += vprintf(format, ap);
#else
    ret += BH_VPRINTF(format, ap);
#endif
    va_end(ap);

    return ret;
}

int
os_vprintf(const char *format, va_list ap)
{
#ifndef BH_VPRINTF
    return vprintf(format, ap);
#else
    return BH_VPRINTF(format, ap);
#endif
}

void *
os_mmap(void *hint, size_t size, int prot, int flags, os_file_handle file)
{
    void *addr;

    if (size >= UINT32_MAX)
        return NULL;

    if ((addr = BH_MALLOC((uint32)size)))
        memset(addr, 0, (uint32)size);

    return addr;
}

int
os_mprotect(void *addr, size_t size, int prot)
{
    return 0;
}

void
os_dcache_flush()
{}

void
os_icache_flush(void *start, size_t len)
{}

os_raw_file_handle
os_invalid_raw_handle(void)
{
    return -1;
}

void
os_munmap(void *addr, size_t size)
{
    return BH_FREE(addr);
}

int custom_strncasecmp(const char *s1, const char *s2, size_t n) {
    size_t i;
    for (i = 0; i < n && s1[i] && s2[i]; ++i) {
        int c1 = tolower(s1[i]);
        int c2 = tolower(s2[i]);
        if (c1 != c2) {
            return c1 - c2; // Return the difference for sorting
        }
    }
    return s1[i] - s2[i]; // Handle strings of different lengths
}
//
//uint64
//os_time_get_boot_us(void)
//{
//    return (uint64)esp_timer_get_time();
//}
//
//uint64
//os_time_thread_cputime_us(void)
//{
//    /* FIXME if u know the right api */
//    return os_time_get_boot_us();
//}
//
//uint8 *
//os_thread_get_stack_boundary(void)
//{
//#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
//    TaskStatus_t pxTaskStatus;
//    vTaskGetInfo(xTaskGetCurrentTaskHandle(), &pxTaskStatus, pdTRUE, eInvalid);
//    return pxTaskStatus.pxStackBase;
//#else // !defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
//    return NULL;
//#endif
//}
//
//void
//os_thread_jit_write_protect_np(bool enabled)
//{}
//
//int
//os_usleep(uint32 usec)
//{
//    return usleep(usec);
//}
//
///* Below parts of readv & writev are ported from Nuttx, under Apache License
// * v2.0 */
//
//ssize_t
//readv(int fildes, const struct iovec *iov, int iovcnt)
//{
//    ssize_t ntotal;
//    ssize_t nread;
//    size_t remaining;
//    uint8_t *buffer;
//    int i;
//
//    /* Process each entry in the struct iovec array */
//
//    for (i = 0, ntotal = 0; i < iovcnt; i++) {
//        /* Ignore zero-length reads */
//
//        if (iov[i].iov_len > 0) {
//            buffer = iov[i].iov_base;
//            remaining = iov[i].iov_len;
//
//            /* Read repeatedly as necessary to fill buffer */
//
//            do {
//                /* NOTE:  read() is a cancellation point */
//
//                nread = read(fildes, buffer, remaining);
//
//                /* Check for a read error */
//
//                if (nread < 0) {
//                    return nread;
//                }
//
//                /* Check for an end-of-file condition */
//
//                else if (nread == 0) {
//                    return ntotal;
//                }
//
//                /* Update pointers and counts in order to handle partial
//                 * buffer reads.
//                 */
//
//                buffer += nread;
//                remaining -= nread;
//                ntotal += nread;
//            } while (remaining > 0);
//        }
//    }
//
//    return ntotal;
//}
//
//ssize_t
//writev(int fildes, const struct iovec *iov, int iovcnt)
//{
//    ssize_t ntotal;
//    ssize_t nwritten;
//    size_t remaining;
//    uint8_t *buffer;
//    int i;
//
//    /* Process each entry in the struct iovec array */
//
//    for (i = 0, ntotal = 0; i < iovcnt; i++) {
//        /* Ignore zero-length writes */
//
//        if (iov[i].iov_len > 0) {
//            buffer = iov[i].iov_base;
//            remaining = iov[i].iov_len;
//
//            /* Write repeatedly as necessary to write the entire buffer */
//
//            do {
//                /* NOTE:  write() is a cancellation point */
//
//                nwritten = write(fildes, buffer, remaining);
//
//                /* Check for a write error */
//
//                if (nwritten < 0) {
//                    return ntotal ? ntotal : -1;
//                }
//
//                /* Update pointers and counts in order to handle partial
//                 * buffer writes.
//                 */
//
//                buffer += nwritten;
//                remaining -= nwritten;
//                ntotal += nwritten;
//            } while (remaining > 0);
//        }
//    }
//
//    return ntotal;
//}
//
//#if OPENAT_SUPPORT
//int
//openat(int fd, const char *pathname, int flags, ...)
//{
//    int new_fd;
//    int ret;
//    char dir_path[DIR_PATH_LEN];
//    char *full_path;
//
//    ret = fcntl(fd, F_GETPATH, dir_path);
//    if (ret != 0) {
//        errno = -EINVAL;
//        return -1;
//    }
//
//    ret = asprintf(&full_path, "%s/%s", dir_path, pathname);
//    if (ret < 0) {
//        errno = ENOMEM;
//        return -1;
//    }
//
//    new_fd = open(full_path, flags);
//    free(full_path);
//
//    return new_fd;
//}
//#else
//int
//openat(int fd, const char *path, int oflags, ...)
//{
//    errno = ENOSYS;
//    return -1;
//}
//#endif
//
//int
//fstatat(int fd, const char *path, struct stat *buf, int flag)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//int
//mkdirat(int fd, const char *path, mode_t mode)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//ssize_t
//readlinkat(int fd, const char *path, char *buf, size_t bufsize)
//{
//    errno = EINVAL;
//    return -1;
//}
//
//int
//linkat(int fd1, const char *path1, int fd2, const char *path2, int flag)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//int
//renameat(int fromfd, const char *from, int tofd, const char *to)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//int
//symlinkat(const char *target, int fd, const char *path)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//int
//unlinkat(int fd, const char *path, int flag)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//int
//utimensat(int fd, const char *path,
//#if UTIMENSAT_TIMESPEC_POINTER
//          const struct timespec *ts,
//#else
//          const struct timespec ts[2],
//#endif
//          int flag)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//DIR *
//fdopendir(int fd)
//{
//    errno = ENOSYS;
//    return NULL;
//}
//
//#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 4, 2)
//int
//ftruncate(int fd, off_t length)
//{
//    errno = ENOSYS;
//    return -1;
//}
//#endif
//
//int
//futimens(int fd,
//#if FUTIMENS_TIMESPEC_POINTER
//         const struct timespec *times
//#else
//         const struct timespec times[2]
//#endif
//)
//{
//    errno = ENOSYS;
//    return -1;
//}
//
//int
//nanosleep(const struct timespec *req, struct timespec *rem)
//{
//    errno = ENOSYS;
//    return -1;
//}
