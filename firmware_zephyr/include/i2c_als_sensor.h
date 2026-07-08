#pragma once

#include <stdint.h>

void i2c_als_init(void);
uint16_t i2c_als_read_lux(void);
void i2c_als_set_lux(uint16_t lux);
uint16_t i2c_als_read_reg(uint8_t reg);
void i2c_als_write_reg(uint8_t reg, uint16_t value);

#define ALS_REG_CONFIG 0x00
#define ALS_REG_LUX 0x01
