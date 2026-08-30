#ifndef I2C1_H
#define I2C1_H

#include <stdint.h>

void i2c1_init(void);
int i2c1_read(uint8_t address, uint8_t *buffer, uint16_t length);
int i2c1_write(uint8_t address, const uint8_t *data, uint16_t length);

#endif