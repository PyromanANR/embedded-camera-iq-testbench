#include "uart_protocol.h"

#if __has_include(<zephyr/kernel.h>)
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#endif

#include <stdio.h>

void uart_protocol_send(const char *packet) {
#if __has_include(<zephyr/sys/printk.h>)
    printk("%s\n", packet);
#else
    printf("%s\n", packet);
#endif
}
