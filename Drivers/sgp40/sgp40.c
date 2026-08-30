#include "sgp40.h"
#include "i2c1.h"
#include "utility.h"
#include "sensirion_gas_index_algorithm.h"

#define SGP40_I2C_ADDR  0x59

void sgp40_init(void)
{
    delay_ms(1);  // mandatory power-up time before first command
}

void sgp40_read(AirQualityData *data)
{
    static GasIndexAlgorithmParams params;  // persists across calls for VOC index tracking
    static bool is_initialized = false;

    uint8_t cmd_buffer[8];

    if (!is_initialized) {
        GasIndexAlgorithm_init(&params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
        is_initialized = true;
    }

    cmd_buffer[0] = 0x26;  // measure_raw_signal command
    cmd_buffer[1] = 0x0F;

    cmd_buffer[2] = 0x80;  // default RH 50% (0x8000)
    cmd_buffer[3] = 0x00;
    cmd_buffer[4] = 0xA2;  // CRC for RH word

    cmd_buffer[5] = 0x66;  // default temp 25°C (0x6666)
    cmd_buffer[6] = 0x66;
    cmd_buffer[7] = 0x93;  // CRC for temp word

    if (i2c1_write(SGP40_I2C_ADDR, cmd_buffer, 8) != 0) {
        return;
    }

    delay_ms(30);  // measurement duration (typ. 25 ms, max 30 ms)

    uint8_t read_buffer[3];

    if (i2c1_read(SGP40_I2C_ADDR, read_buffer, 3) == 0) {
        uint8_t calculated_crc = sgp40_calculate_crc(read_buffer, 2);

        if (calculated_crc == read_buffer[2]) {
            uint16_t raw_voc = ((uint16_t)read_buffer[0] << 8) | read_buffer[1];
            int32_t calculated_index = 0;

            GasIndexAlgorithm_process(&params, raw_voc, &calculated_index);  // convert raw signal to VOC index
            data->voc = calculated_index;
        }
    }
}

uint8_t sgp40_calculate_crc(const uint8_t *data, uint16_t length)
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