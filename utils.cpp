#include <cstring>
#include <sys/time.h>
#include <cinttypes>
#include "utils.h"
#include "err.h"
#include "crc32.h"

uint32_t get_number(char *str_num) {
    size_t len = strlen(str_num);

    for (size_t i = 0; i < len; i++)
        if (!isdigit(str_num[i]))
            syserr("not a digit sign");

    unsigned long result =  strtoul(str_num, NULL, 10);
    if (!result || errno == ERANGE || result > UINT32_MAX) /* overflow */
        syserr("Wrong number");
    return (uint32_t) result;
}

uint32_t crc32(char *data, uint32_t len) {
    return crc_32(data, len);
}

uint64_t get_time_microseconds() {
    struct timeval *tv = new struct timeval;
    gettimeofday(tv, nullptr);
    return (uint64_t)(tv->tv_sec * 1000000LL + tv->tv_usec);
}

char *get_string_of_32bit(uint32_t number) {
    char* result = new char[16];
    sprintf(result,"%" PRIu32, number);
    return result;
}
