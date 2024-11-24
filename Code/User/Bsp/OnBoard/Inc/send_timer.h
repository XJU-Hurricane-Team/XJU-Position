/**
 * @file    send_timer.h
 * @author  Deadline039
 * @brief   周期发送数据
 * @version 1.0
 * @date    2024-09-30
 */

#ifndef __SEND_TIMER_H
#define __SEND_TIMER_H

#include "stm32f1xx_hal.h"

#define SEND_TIMER                 TIM2
#define SEND_TIMER_CLK_ENABLE()    __HAL_RCC_TIM2_CLK_ENABLE()
#define SEND_TIMER_IRQn            TIM2_IRQn
#define SEND_TIMER_IRQHandler      TIM2_IRQHandler

/* 记录发送频率存储地址 */
#define SEND_TIMER_STORAGE_ADDRESS 0x0800C000

uint32_t send_timer_init(pTIM_CallbackTypeDef send_callback);
void send_timer_enable(void);
void send_timer_disable(void);
void send_timer_set_frequency(uint32_t freq);

#endif /* __SEND_TIMER_H */
