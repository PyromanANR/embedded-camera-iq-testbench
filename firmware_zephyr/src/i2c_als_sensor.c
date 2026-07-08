#include "i2c_als_sensor.h"

static uint16_t registers[2];

void i2c_als_init(void) {
    registers[ALS_REG_CONFIG] = 0x0001;
    registers[ALS_REG_LUX] = 320;
}

uint16_t i2c_als_read_lux(void) {
    return registers[ALS_REG_LUX];
}

void i2c_als_set_lux(uint16_t lux) {
    registers[ALS_REG_LUX] = lux;
}

uint16_t i2c_als_read_reg(uint8_t reg) {
    return registers[reg & 0x01];
}

void i2c_als_write_reg(uint8_t reg, uint16_t value) {
    registers[reg & 0x01] = value;
}
