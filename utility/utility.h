#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>

extern volatile uint32_t system_ticks;

void systick_init(uint32_t system_clock_hz);
uint32_t get_systick_ms(void);
void delay_ms(uint32_t ms);

void uint16_to_str(uint16_t value, char *buffer);
int uint32_to_str(uint32_t value, char *buffer);
void float_to_str(float value, char *buffer, int decimals);

#endif