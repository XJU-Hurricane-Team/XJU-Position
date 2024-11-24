/**
 * @file    led.h
 * @author  Deadline039
 * @brief   On board LED.
 * @version 1.1
 * @date    2024-07-29
 */

#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "bsp_core.h"

#define LED_RED_GPIO_PORT       GPIOB
#define LED_RED_GPIO_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_RED_GPIO_PIN        GPIO_PIN_9

#define LED_GREEN_GPIO_PORT     GPIOB
#define LED_GREEN_GPIO_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_GREEN_GPIO_PIN      GPIO_PIN_10

/* LED_RED */
#define LED_RED(x)                                                             \
    x ? HAL_GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN, GPIO_PIN_SET)   \
      : HAL_GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN, GPIO_PIN_RESET)
#define LED_RED_ON()                                                           \
    HAL_GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN, GPIO_PIN_RESET)
#define LED_RED_OFF()                                                          \
    HAL_GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN, GPIO_PIN_SET)
#define LED_RED_TOGGLE() HAL_GPIO_TogglePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN)

/* LED_GREEN */
#define LED_GREEN(x)                                                           \
    x ? HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN,             \
                          GPIO_PIN_SET)                                        \
      : HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN,             \
                          GPIO_PIN_RESET)
#define LED_GREEN_ON()                                                         \
    HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN, GPIO_PIN_RESET)
#define LED_GREEN_OFF()                                                        \
    HAL_GPIO_WritePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN, GPIO_PIN_SET)
#define LED_GREEN_TOGGLE()                                                     \
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN)

void led_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LED_H */
