/**
 * @file    send_timer.c
 * @author  Deadline039
 * @brief   周期发送定时器
 * @version 1.0
 * @date    2024-09-30
 */

#include "send_timer.h"

#include <assert.h>
#include <stdio.h>

static TIM_HandleTypeDef send_timer_handle;

/**
 * @brief 定时器初始化
 *
 * @param send_callback 定时发送执行的函数
 * @return flash中存储的频率
 */
uint32_t send_timer_init(pTIM_CallbackTypeDef send_callback) {
    HAL_StatusTypeDef res = HAL_OK;
    uint32_t freq = *(uint32_t *)SEND_TIMER_STORAGE_ADDRESS;

    if (freq < 1 || freq > 200) {
        /* 默认, 最大 200 Hz */
        send_timer_set_frequency(200);
    }

    send_timer_handle.Instance = SEND_TIMER;
    send_timer_handle.Init.Period = (uint32_t)(10000 / freq);
    send_timer_handle.Init.Prescaler = 7199;
    send_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    send_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;

    SEND_TIMER_CLK_ENABLE();
    res = HAL_TIM_Base_Init(&send_timer_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    HAL_NVIC_SetPriority(SEND_TIMER_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(SEND_TIMER_IRQn);

    HAL_TIM_RegisterCallback(&send_timer_handle, HAL_TIM_PERIOD_ELAPSED_CB_ID,
                             send_callback);

    send_timer_enable();

    return freq;
}

/**
 * @brief 开启定时发送
 *
 */
void send_timer_enable(void) {
    HAL_TIM_Base_Start_IT(&send_timer_handle);
}

/**
 * @brief 关闭定时发送
 *
 */
void send_timer_disable(void) {
    HAL_TIM_Base_Stop_IT(&send_timer_handle);
}

/**
 * @brief 设置发送频率
 *
 * @param freq 发送频率
 */
void send_timer_set_frequency(uint32_t freq) {
    HAL_StatusTypeDef res = HAL_OK;
    FLASH_EraseInitTypeDef erase_init_struct;
    uint32_t error_code;

    HAL_FLASH_Unlock();
    erase_init_struct.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init_struct.NbPages = 1;
    erase_init_struct.PageAddress = SEND_TIMER_STORAGE_ADDRESS;
    res = HAL_FLASHEx_Erase(&erase_init_struct, &error_code);

    if (res != HAL_OK) {
        fprintf(stderr, "Erase flash error! error code: %X\r\n", error_code);
        return;
    }

    res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, SEND_TIMER_STORAGE_ADDRESS,
                            freq);

    if (res != HAL_OK) {
        fprintf(stderr, "Write flash error! \r\n");
        return;
    }

    if (HAL_TIM_Base_GetState(&send_timer_handle) != HAL_TIM_STATE_RESET) {
        send_timer_disable();
        send_timer_init(send_timer_handle.PeriodElapsedCallback);
    }
}

/**
 * @brief 周期发送定时器中断服务函数
 *
 */
void SEND_TIMER_IRQHandler(void) {
    HAL_TIM_IRQHandler(&send_timer_handle);
}
