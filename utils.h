#ifndef NETWORKS_NETACKA_UTILS_H
#define NETWORKS_NETACKA_UTILS_H
#include <iostream>

/**
 * If it is not unsigned 4bytes, exits with code 1 and proper message
 * @param str_num represented as a string
 * @return unsigned number
 */
uint32_t get_number(char *str_num);

/**
 * Calculate checksum csrc32
 * @param data
 * @param len
 * @return checksum
 */
uint32_t crc32(char *data, uint32_t len); /* TODO Not implemented */

#endif //NETWORKS_NETACKA_UTILS_H
