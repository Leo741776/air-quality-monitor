#ifndef ST7735_H
#define ST7735_H

#include "aqd.h"

void st7735_init(void);
void st7735_update(const AirQualityData *data);

#endif