#include "aqd.h"
#include "main.h"

void get_snapshot(const AirQualityData *global_data, AirQualityData *fresh_data)
{
    uint32_t primask = __get_PRIMASK();  // save current interrupt state
    __disable_irq();                     // enter critical section

    *fresh_data = *global_data;          // atomic copy of shared data

    __set_PRIMASK(primask);              // restore interrupt state
}