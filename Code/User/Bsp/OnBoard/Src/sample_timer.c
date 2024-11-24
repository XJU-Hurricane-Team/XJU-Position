/**
 * @file    sample_timer.c
 * @author  Deadline039
 * @brief   采样定时器
 * @version 1.0
 * @date    2024-11-17
 */

#include "sample_timer.h"

static TIM_HandleTypeDef sample_timer_handle;

uint8_t g_position_calc_flag;

/**
 * @brief 采样定时器中断回调
 *
 * @param htim 定时器句柄
 */
static void sample_timer_callback(TIM_HandleTypeDef *htim) {
    if (htim->Instance != SAMPLE_TIMER) {
        return;
    }

    g_position_calc_flag = 1;
}

/**
 * @brief 编码器采样定时器初始化
 *
 * @param period 周期(ms)
 */
void sample_timer_init(uint32_t period) {
    HAL_StatusTypeDef res = HAL_OK;

    sample_timer_handle.Instance = SAMPLE_TIMER;
    sample_timer_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    sample_timer_handle.Init.Prescaler = 7200 - 1;
    sample_timer_handle.Init.Period = period * 10 - 1;
    sample_timer_handle.Init.CounterMode = TIM_COUNTERMODE_UP;

    SAMPLE_TIMER_CLK_ENABLE();

    res = HAL_TIM_Base_Init(&sample_timer_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */


    HAL_NVIC_SetPriority(SAMPLE_TIMER_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(SAMPLE_TIMER_IRQn);

    HAL_TIM_RegisterCallback(&sample_timer_handle, HAL_TIM_PERIOD_ELAPSED_CB_ID,
                             sample_timer_callback);
    sample_timer_enable();
}

/**
 * @brief 采样定时器中断服务函数
 *
 */
void SAMPLE_TIMER_IRQHandler(void) {
    HAL_TIM_IRQHandler(&sample_timer_handle);
}

/**
 * @brief 开启采样
 *
 */
void sample_timer_enable(void) {
    HAL_TIM_Base_Start_IT(&sample_timer_handle);
}

/**
 * @brief 关闭采样
 *
 */
void sample_timer_disable(void) {
    HAL_TIM_Base_Stop_IT(&sample_timer_handle);
}