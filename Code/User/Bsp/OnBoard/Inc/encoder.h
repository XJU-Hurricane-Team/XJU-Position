/**
 * @file    encoder.h
 * @author  Deadline039
 * @brief   编码器计数
 * @version 1.0
 * @date    2024-09-29
 */

#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f1xx_hal.h"

#define ENCODER_TIM_PERIOD    1
#define ENCODER_TIM_PRESCALER 0

#define ENCODER_MODE          TIM_ENCODERMODE_TI12

#define ENCODER_IC1_POLARITY  TIM_ICPOLARITY_RISING
#define ENCODER_IC2_POLARITY  TIM_ICPOLARITY_RISING

#define ENCODER_RESOLUTION    12

#if (ENCODER_MODE == TIM_ENCODERMODE_TI12)
#define ENCODER_TOTAL_RESOLUTION                                               \
    (ENCODER_RESOLUTION * 4) /* 4倍频后的总分辨率 */
#else                        /* ENCODER_MODE */
#define ENCODER_TOTAL_RESOLUTION                                               \
    (ENCODER_RESOLUTION * 2) /* 2倍频后的总分辨率 */
#endif                       /* ENCODER_MODE */

/* 一圈的周期数 */
#define ENCODER_CIRCLE_PERIOD     1024

/**
 * @defgroup 编码器0
 * @{
 */

#define ENCODER0_TIM              TIM3
#define ENCODER0_TIM_CLK_ENABLE() __HAL_RCC_TIM3_CLK_ENABLE()

#define ENCODER0_TIM_IRQn         TIM3_IRQn
#define ENCODER0_TIM_IRQHandler   TIM3_IRQHandler

#define ENCODER0_CHA_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define ENCODER0_CHA_PORT         GPIOA
#define ENCODER0_CHA_PIN          GPIO_PIN_6
#define ENCODER0_CHB_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define ENCODER0_CHB_PORT         GPIOA
#define ENCODER0_CHB_PIN          GPIO_PIN_7
#define ENCODER0_CHI_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define ENCODER0_CHI_PORT         GPIOA
#define ENCODER0_CHI_PIN          GPIO_PIN_8

extern int32_t g_encoder0_count;

/**
 * @}
 */

/**
 * @defgroup 编码器1
 * @{
 */
#define ENCODER1_TIM              TIM4
#define ENCODER1_TIM_CLK_ENABLE() __HAL_RCC_TIM4_CLK_ENABLE()

#define ENCODER1_TIM_IRQn         TIM4_IRQn
#define ENCODER1_TIM_IRQHandler   TIM4_IRQHandler

#define ENCODER1_CHA_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ENCODER1_CHA_PORT         GPIOB
#define ENCODER1_CHA_PIN          GPIO_PIN_6
#define ENCODER1_CHB_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ENCODER1_CHB_PORT         GPIOB
#define ENCODER1_CHB_PIN          GPIO_PIN_7
#define ENCODER1_CHI_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ENCODER1_CHI_PORT         GPIOB
#define ENCODER1_CHI_PIN          GPIO_PIN_8

extern int32_t g_encoder1_count;

/**
 * @}
 */

void encoder_init(void);

#endif /* __ENCODER_H */