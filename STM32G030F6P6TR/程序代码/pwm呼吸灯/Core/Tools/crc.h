#ifndef __CRC__H
#define __CRC__H

#include "main.h"

extern const uint8_t crc8_table[];
//extern const uint16_t crc16_table[];

uint8_t crc8(uint8_t *data, uint16_t length);
//uint16_t crc16(uint8_t *data, uint16_t length);
//uint16_t to_low_in_the_first( uint16_t data );
//uint32_t crc32( uint8_t *data, uint32_t size, uint32_t crc );

#endif




