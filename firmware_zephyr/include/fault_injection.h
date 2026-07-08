#pragma once

#include <stdint.h>

uint8_t fault_injection_error_code(void);
void fault_injection_set_error(uint8_t error_code);

