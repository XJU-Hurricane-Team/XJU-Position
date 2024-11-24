/**
 * @file    led.c
 * @author  Deadline039
 * @brief   LED驱动代码
 * @version 1.1
 * @date    2023-10-20
 */

#include "led.h"

/**
 * @brief LED初始化
 *
 */
void led_init(void) {
    GPIO_InitTypeDef gpio_initure = {0};

    LED_RED_GPIO_ENABLE();

    gpio_initure.Pin = LED_RED_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_RED_GPIO_PORT, &gpio_initure);

    LED_GREEN_GPIO_ENABLE();
    gpio_initure.Pin = LED_GREEN_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_GREEN_GPIO_PORT, &gpio_initure);

    /* 关闭LED */
    LED_RED_OFF();
    LED_GREEN_OFF();
}
