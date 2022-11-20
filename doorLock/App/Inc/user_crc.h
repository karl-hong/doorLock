#ifndef _USER_CRC_H__
#define _USER_CRC_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


uint8_t cal_crc8(const char *ptr, unsigned int len);
uint16_t cal_crc16(const char *ptr, int len);
uint32_t cal_crc32(const uint8_t *ptr, int len);

#endif
