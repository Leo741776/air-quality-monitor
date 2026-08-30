#ifndef AQD_H
#define AQD_H

#include <stdint.h>

typedef struct {
	uint16_t pm1_0;
	uint16_t pm2_5;
	uint16_t pm10;
	uint16_t pm2_5_env;
	uint16_t co2;
	int32_t voc;
	float temperature;
	float humidity;
} AirQualityData;

void get_snapshot(
    const AirQualityData *global_data,
    AirQualityData *fresh_data
);

#endif