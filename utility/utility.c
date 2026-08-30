#include "utility.h"
#include "main.h"
#include <stdint.h>

volatile uint32_t system_ticks = 0;

void systick_init(uint32_t system_clock_hz) {
    SysTick->LOAD = (system_clock_hz / 1000U) - 1U;  // 1ms tick period
    SysTick->VAL = 0;

    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

uint32_t get_systick_ms(void)
{
    return system_ticks;
}

void delay_ms(uint32_t ms)
{
    uint32_t start_tick = get_systick_ms();

    while ((get_systick_ms() - start_tick) < ms) {}  // busy-wait
}

void SysTick_Handler(void)
{
    system_ticks++;
}

void uint16_to_str(uint16_t value, char *buffer)
{
    char temp[6];
    int i = 0;
    int j = 0;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0) {           // extract digits in reverse order
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i > 0) {                // reverse into output buffer
        buffer[j++] = temp[--i];
    }

    buffer[j] = '\0';
}

int uint32_to_str(uint32_t value, char *buffer) {
    char temp[10];
    int i = 0;
    int j = 0;

    if (value == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return 1;
    }

    while (value > 0) {           // extract digits in reverse order
        temp[i++] = (value % 10) + '0';
        value /= 10;
    }

    int len = i;
    while (i > 0) {                // reverse into output buffer
        buffer[j++] = temp[--i];
    }
    buffer[j] = '\0';
    return len;
}

void float_to_str(float value, char *buffer, int decimals)
{
    int idx = 0;

    if (value < 0) {
        buffer[idx++] = '-';
        value = -value;
    }

    uint32_t int_part = (uint32_t)value;
    float frac_part = value - (float)int_part;

    char int_buf[12];
    int int_len = uint32_to_str(int_part, int_buf);

    for (int k = 0; k < int_len; k++) {
        buffer[idx++] = int_buf[k];
    }

    if (decimals > 0) {
        buffer[idx++] = '.';

        uint32_t scale = 1;

        for (int k = 0; k < decimals; k++) {   // compute 10^decimals
            scale *= 10;
        }

        uint32_t frac_scaled =
            (uint32_t)(frac_part * (float)scale + 0.5f);  // rounded fractional value

        if (frac_scaled >= scale) {    // handle rounding overflow into integer part
            frac_scaled -= scale;
            int_part += 1;
        }

        char frac_buf[12];
        int frac_len = uint32_to_str(frac_scaled, frac_buf);

        for (int k = frac_len; k < decimals; k++) {  // zero-pad to fixed width
            buffer[idx++] = '0';
        }

        for (int k = 0; k < frac_len; k++) {
            buffer[idx++] = frac_buf[k];
        }
    }

    buffer[idx] = '\0';
}