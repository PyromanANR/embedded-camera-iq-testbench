#include "fault_injection.h"

static uint8_t simulated_error_code = 0;

uint8_t fault_injection_error_code(void) {
    return simulated_error_code;
}

void fault_injection_set_error(uint8_t error_code) {
    simulated_error_code = error_code;
}

