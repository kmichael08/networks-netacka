#include <cstring>
#include <sys/time.h>
#include "utils.h"
#include "err.h"
#include "crc32.h"

uint32_t get_number(char *str_num) {
    if (strchr(str_num, '-') != NULL)
        syserr("Not a non-negative number");
    uint32_t result = (uint32_t) strtoul(str_num, NULL, 10);
    if (!result || errno == ERANGE) /* overflow */
        syserr("Wrong number");
    return result;
}

uint32_t crc32(char *data, uint32_t len) {
    return crc_32(data, len);
}

uint64_t get_time_microseconds() {
    struct timeval *tv = new struct timeval;
    gettimeofday(tv, nullptr);
    return (uint64_t)(tv->tv_sec * 1000000LL + tv->tv_usec);
}