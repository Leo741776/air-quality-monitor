#ifndef PMS5003_H
#define PMS5003_H

#include "aqd.h"

void pms5003_init(void);
int pms5003_read(AirQualityData *data);

#endif