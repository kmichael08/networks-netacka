#ifndef NETWORKS_NETACKA_UTILS_H
#define NETWORKS_NETACKA_UTILS_H
#include <iostream>
#include "err.h"

uint32_t get_number(char *str_num) {
    if (strchr(str_num, '-') != NULL)
        syserr("Not a non-negative number");
    uint32_t result = (uint32_t) strtoul(str_num, NULL, 10);
    if (!result || errno == ERANGE)
        syserr("Wrong number");
    return result;
}

#endif //NETWORKS_NETACKA_UTILS_H
