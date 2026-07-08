#include "spi_image_sensor.h"

static uint16_t registers[16];

void spi_image_sensor_init(void) {
    registers[IMAGE_SENSOR_REG_ID] = 0x5343;
    registers[IMAGE_SENSOR_REG_EXPOSURE] = 83;
    registers[IMAGE_SENSOR_REG_GAIN] = 14;
    registers[IMAGE_SENSOR_REG_FRAME_STATUS] = 0;
}

uint16_t spi_image_sensor_read_reg(uint8_t reg) {
    return registers[reg & 0x0F];
}

void spi_image_sensor_write_reg(uint8_t reg, uint16_t value) {
    registers[reg & 0x0F] = value;
}
