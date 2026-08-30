#ifndef SGP40_H
#define SGP40_H

#include "aqd.h"

void sgp40_init(void);
void sgp40_read(AirQualityData *data);
uint8_t sgp40_calculate_crc(const uint8_t *data, uint16_t length);

#endif