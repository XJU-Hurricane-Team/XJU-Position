/**
 * @file    sample_timer.h
 * @author  Deadline039
 * @brief   采样定时器
 * @version 1.0
 * @date    2024-11-17
 */

#include "stm32f1xx_hal.h"

#define SAMPLE_TIMER              TIM3
#define SAMPLE_TIMER_CLK_ENABLE() __HAL_RCC_TIM3_CLK_ENABLE()
#define SAMPLE_TIMER_IRQn         TIM3_IRQn
#define SAMPLE_TIMER_IRQHandler   TIM3_IRQHandler

extern uint8_t g_position_calc_flag;

void sample_timer_init(uint32_t period);
void sample_timer_enable(void);
void sample_timer_disable(void);
