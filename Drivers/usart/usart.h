#ifndef USART1_H
#define USART1_H

#include <stdint.h>

void usart1_init(void);
int usart1_read(uint8_t *byte, uint32_t timeout_ms);

#endif