#include "scd40.h"
#include "utility.h"
#include "i2c1.h"
#include <stdint.h>

#define SCD40_I2C_ADDR          0x62
#define CMD_READ_MEASUREMENT    0xEC05
#define CMD_START_MEASUREMENT   0x21B1
#define CMD_GET_DATA_READY      0xE4B8

void scd40_init(void)
{
    uint8_t cmd[2] = {
        (CMD_START_MEASUREMENT >> 8) & 0xFF,
        CMD_START_MEASUREMENT & 0xFF
    };

    i2c1_write(SCD40_I2C_ADDR, cmd, 2);  // start periodic measurement mode

    delay_ms(2);  // allow sensor to process command
}

void scd40_read(AirQualityData *data)
{
    if (!scd40_is_data_ready()) {
        return;  // skip read if no new sample yet, avoids NACK
    }

    uint8_t buffer[9];

    uint8_t cmd[2] = {
        (CMD_READ_MEASUREMENT >> 8) & 0xFF,
        CMD_READ_MEASUREMENT & 0xFF
    };

    if (i2c1_write(SCD40_I2C_ADDR, cmd, 2) != 0) {
        return;
    }

    delay_ms(1);  // processing time before read

    if (i2c1_read(SCD40_I2C_ADDR, buffer, 9) != 0) {
        return;
    }

    // verify CRC8 for each 2-byte word (CO2, temp, humidity)
    if (scd40_calculate_crc(&buffer[0], 2) != buffer[2] ||
        scd40_calculate_crc(&buffer[3], 2) != buffer[5] ||
        scd40_calculate_crc(&buffer[6], 2) != buffer[8]) {
        return;
    }

    uint16_t raw_co2 = ((uint16_t)buffer[0] << 8) | buffer[1];
    uint16_t raw_temp = ((uint16_t)buffer[3] << 8) | buffer[4];
    uint16_t raw_hum = ((uint16_t)buffer[6] << 8) | buffer[7];

    data->co2 = raw_co2;
    data->temperature = -49.0f + (315.0f * (float)raw_temp) / 65535.0f;  // datasheet conversion
    data->humidity = (100.0f * (float)raw_hum) / 65535.0f;               // datasheet conversion
}

uint8_t scd40_calculate_crc(const uint8_t *data, uint16_t length)
{
    uint8_t crc = 0xFF;
    const uint8_t pattern = 0x31;  // CRC8 polynomial per Sensirion datasheet

    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];

        for (uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ pattern;
            }
            else {
                crc = (crc << 1);
            }
        }
    }

    return crc;
}

bool scd40_is_data_ready(void)
{
    uint8_t cmd[2] = {0xE4, 0xB8};
    uint8_t buffer[3];

    if (i2c1_write(SCD40_I2C_ADDR, cmd, 2) != 0) {
        return false;
    }

    delay_ms(1);  // processing time for status command

    if (i2c1_read(SCD40_I2C_ADDR, buffer, 3) != 0) {
        return false;
    }

    uint16_t status = ((uint16_t)buffer[0] << 8) | buffer[1];

    return (status & 0x07FF) != 0;  // lower 11 bits set = data ready
}