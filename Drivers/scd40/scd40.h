#ifndef SCD40_H
#define SCD40_H

#include "aqd.h"
#include <stdbool.h>

void scd40_init(void);
void scd40_read(AirQualityData *data);
uint8_t scd40_calculate_crc(const uint8_t *data, uint16_t length);
bool scd40_is_data_ready(void);

#endif