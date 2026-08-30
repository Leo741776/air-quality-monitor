#include "st7735.h"
#include "st7735_library.h"
#include "utility.h"

#define PORTRAIT_MODE 0
#define START_X       10
#define START_Y       10
#define LABEL_OFFSET  60
#define Y_SPACING     15
#define NUM_DATA      8

#define BACKGROUND_START_X  0
#define BACKGROUND_START_Y  0
#define BACKGROUND_WIDTH    128
#define BACKGROUND_HEIGHT   160

extern FontDef Font_7x10;

void st7735_init(void)
{
    ST7735_Init(PORTRAIT_MODE);

    ST7735_FillRectangle(BACKGROUND_START_X, BACKGROUND_START_Y, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, BLACK);

    const char *labels[NUM_DATA] = {" PM1.0:", " PM2.5:", "  PM10:", "PM2.5E:", "   CO2:", "   VOC:", "  TEMP:", "    RH:"};

    for (int i = 0; i < NUM_DATA; i++) {   // draw static labels once at startup
        ST7735_WriteString(
            START_X,
            START_Y + (i * Y_SPACING),
            labels[i],
            Font_7x10,
            CYAN,
            BLACK
        );
    }
}

void st7735_update(const AirQualityData *data)
{
    char string_pm1_0[6];
    char string_pm2_5[6];
    char string_pm10[6];
    char string_pm2_5_env[6];
    char string_co2[6];
    char string_voc[6];
    char string_temperature[12];
    char string_humidity[12];

    // convert sensor values to display strings
    uint16_to_str(data->pm1_0, string_pm1_0);
    uint16_to_str(data->pm2_5, string_pm2_5);
    uint16_to_str(data->pm10, string_pm10);
    uint16_to_str(data->pm2_5_env, string_pm2_5_env);
    uint16_to_str(data->co2, string_co2);
    uint16_to_str(data->voc, string_voc);
    float_to_str(data->temperature, string_temperature, 1);
    float_to_str(data->humidity, string_humidity, 1);

    const char *lines[NUM_DATA];

    lines[0] = string_pm1_0;
    lines[1] = string_pm2_5;
    lines[2] = string_pm10;
    lines[3] = string_pm2_5_env;
    lines[4] = string_co2;
    lines[5] = string_voc;
    lines[6] = string_temperature;
    lines[7] = string_humidity;

    for (int i = 0; i < NUM_DATA; i++) {   // redraw values in place next to labels
        ST7735_WriteString(
            START_X + LABEL_OFFSET,
            START_Y + (i * Y_SPACING),
            lines[i],
            Font_7x10,
            WHITE,
            BLACK
        );
    }
}