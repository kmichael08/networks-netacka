#include <cstring>
#include "utils.h"
#include "err.h"

uint32_t get_number(char *str_num) {
    if (strchr(str_num, '-') != NULL)
        syserr("Not a non-negative number");
    uint32_t result = (uint32_t) strtoul(str_num, NULL, 10);
    if (!result || errno == ERANGE)
        syserr("Wrong number");
    return result;
}

uint32_t csrc32(char* data, uint32_t len) {}