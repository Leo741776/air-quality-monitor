#include "pms5003.h"
#include "usart.h"
#include "utility.h"
#include <stdint.h>

#define FRAME_LEN 32
#define DATA_LEN  30

void pms5003_init(void)
{
    delay_ms(100);  // stabilization delay for sensor power-up
}

int pms5003_read(AirQualityData *data)
{
    uint8_t buffer[FRAME_LEN];
    uint32_t calculated_checksum = 0;
    uint32_t received_checksum = 0;
    uint8_t b = 0;

    // scan for start-of-frame byte 0x42
    while (1) {
        if (usart1_read(&b, 2000) != 0) {
            return -1;
        }

        if (b == 0x42) {
            buffer[0] = b;
            break;
        }
    }

    // confirm second header byte 0x4D
    if (usart1_read(&b, 50) != 0 || b != 0x4D) {
        return -1;
    }

    buffer[1] = b;

    // read remaining 30 bytes of the frame
    for (int i = 2; i < 32; i++) {
        if (usart1_read(&buffer[i], 50) != 0) {
            return -1;
        }
    }

    // sum first 30 bytes for checksum validation
    for (int i = 0; i < DATA_LEN; i++) {
        calculated_checksum += buffer[i];
    }

    received_checksum =
        ((uint16_t)buffer[DATA_LEN] << 8) | buffer[DATA_LEN + 1];  // checksum sent in frame

    if (calculated_checksum != received_checksum) {
        return -1;
    }

    // extract PM1.0, PM2.5, PM10, and environmental PM2.5 fields
    data->pm1_0 = ((uint16_t)buffer[4] << 8) | buffer[5];
    data->pm2_5 = ((uint16_t)buffer[6] << 8) | buffer[7];
    data->pm10 = ((uint16_t)buffer[8] << 8) | buffer[9];
    data->pm2_5_env = ((uint16_t)buffer[12] << 8) | buffer[13];

    return 0;
}