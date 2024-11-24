/**
 * @file    bsp.c
 * @author  Deadline039
 * @brief   Bsp layer initialize.
 * @version 1.0
 * @date    2024-09-18
 */

#include "bsp.h"

/**
 * @brief Bsp layer initiallize.
 *
 */
void bsp_init(void) {
    HAL_Init();
    system_clock_config();
    delay_init(72);
    usart1_init(115200);
    usart2_init(9600);
    led_init();
    key_init();
    spi_init();
    as5047_init();
}