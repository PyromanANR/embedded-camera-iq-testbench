#pragma once

#include <stdint.h>

void spi_image_sensor_init(void);
uint16_t spi_image_sensor_read_reg(uint8_t reg);
void spi_image_sensor_write_reg(uint8_t reg, uint16_t value);

#define IMAGE_SENSOR_REG_ID 0x00
#define IMAGE_SENSOR_REG_EXPOSURE 0x01
#define IMAGE_SENSOR_REG_GAIN 0x02
#define IMAGE_SENSOR_REG_FRAME_STATUS 0x03
