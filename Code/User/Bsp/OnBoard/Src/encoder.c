/**
 * @file    encoder.c
 * @author  Deadline039
 * @brief   编码器计数
 * @version 1.0
 * @date    2024-09-29
 */

#include "encoder.h"

#include <assert.h>

int32_t g_encoder0_count = 0;
TIM_HandleTypeDef encoder0_tim_handle;

int32_t g_encoder1_count = 0;
TIM_HandleTypeDef encoder1_tim_handle;

/**
 * @brief 编码器溢出回调
 *
 * @param htim 定时器句柄
 * @note 两个编码器镜像安装, 编码器0跟1计数相反
 */
static void encoder_callback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == ENCODER0_TIM) {
        (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) ? (++g_encoder0_count)
                                               : (--g_encoder0_count);
    } else if (htim->Instance == ENCODER1_TIM) {
        (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) ? (--g_encoder1_count)
                                               : (++g_encoder1_count);
    }
}

/**
 * @brief 编码器初始化
 *
 */
void encoder_init(void) {
    HAL_StatusTypeDef res = HAL_OK;
    TIM_Encoder_InitTypeDef encoder_config;

    ENCODER0_TIM_CLK_ENABLE();

    encoder0_tim_handle.Instance = ENCODER0_TIM;
    encoder0_tim_handle.Init.Prescaler = ENCODER_TIM_PRESCALER;
    encoder0_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    encoder0_tim_handle.Init.Period = ENCODER_TIM_PERIOD;
    encoder0_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    encoder0_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    encoder_config.EncoderMode = ENCODER_MODE;

    encoder_config.IC1Polarity = ENCODER_IC1_POLARITY;
    encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC1Filter = 0;

    encoder_config.IC2Polarity = ENCODER_IC2_POLARITY;
    encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC2Filter = 0;

    res = HAL_TIM_Encoder_Init(&encoder0_tim_handle, &encoder_config);
#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    HAL_TIM_RegisterCallback(&encoder0_tim_handle, HAL_TIM_PERIOD_ELAPSED_CB_ID,
                             encoder_callback);

    __HAL_TIM_SET_COUNTER(&encoder0_tim_handle, 0);

    __HAL_TIM_CLEAR_IT(&encoder0_tim_handle, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&encoder0_tim_handle, TIM_IT_UPDATE);
    __HAL_TIM_URS_ENABLE(&encoder0_tim_handle);

    HAL_TIM_Encoder_Start(&encoder0_tim_handle, TIM_CHANNEL_ALL);

    ENCODER1_TIM_CLK_ENABLE();

    encoder1_tim_handle.Instance = ENCODER1_TIM;
    encoder1_tim_handle.Init.Prescaler = ENCODER_TIM_PRESCALER;
    encoder1_tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    encoder1_tim_handle.Init.Period = ENCODER_TIM_PERIOD;
    encoder1_tim_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    encoder1_tim_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    encoder_config.EncoderMode = ENCODER_MODE;
    encoder_config.IC1Polarity = ENCODER_IC1_POLARITY;
    encoder_config.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC1Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC1Filter = 0;
    encoder_config.IC2Polarity = ENCODER_IC2_POLARITY;
    encoder_config.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_config.IC2Prescaler = TIM_ICPSC_DIV1;
    encoder_config.IC2Filter = 0;
    res = HAL_TIM_Encoder_Init(&encoder1_tim_handle, &encoder_config);
#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    HAL_TIM_RegisterCallback(&encoder1_tim_handle, HAL_TIM_PERIOD_ELAPSED_CB_ID,
                             encoder_callback);

    __HAL_TIM_SET_COUNTER(&encoder1_tim_handle, 0);

    __HAL_TIM_CLEAR_IT(&encoder1_tim_handle, TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT(&encoder1_tim_handle, TIM_IT_UPDATE);
    __HAL_TIM_URS_ENABLE(&encoder1_tim_handle);

    HAL_TIM_Encoder_Start(&encoder1_tim_handle, TIM_CHANNEL_ALL);
}

/**
 * @brief 定时器AB相编码底层驱动函数
 *
 * @param htim 定时器句柄
 */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef gpio_init_struct = {0};

    gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (htim->Instance == ENCODER0_TIM) {
        ENCODER0_CHA_CLK_ENABLE();
        ENCODER0_CHB_CLK_ENABLE();
        ENCODER0_CHI_CLK_ENABLE();
        gpio_init_struct.Pin = ENCODER0_CHA_PIN;
        HAL_GPIO_Init(ENCODER0_CHA_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = ENCODER0_CHB_PIN;
        HAL_GPIO_Init(ENCODER0_CHB_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = ENCODER0_CHI_PIN;
        HAL_GPIO_Init(ENCODER0_CHI_PORT, &gpio_init_struct);

        HAL_NVIC_SetPriority(ENCODER0_TIM_IRQn, 5, 1);
        HAL_NVIC_EnableIRQ(ENCODER0_TIM_IRQn);
    } else if (htim->Instance == ENCODER1_TIM) {
        ENCODER1_CHA_CLK_ENABLE();
        ENCODER1_CHB_CLK_ENABLE();
        ENCODER1_CHI_CLK_ENABLE();

        gpio_init_struct.Pin = ENCODER1_CHA_PIN;
        HAL_GPIO_Init(ENCODER1_CHA_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = ENCODER1_CHB_PIN;
        HAL_GPIO_Init(ENCODER1_CHB_PORT, &gpio_init_struct);

        gpio_init_struct.Pin = ENCODER1_CHI_PIN;
        HAL_GPIO_Init(ENCODER1_CHI_PORT, &gpio_init_struct);

        HAL_NVIC_SetPriority(ENCODER1_TIM_IRQn, 5, 1);
        HAL_NVIC_EnableIRQ(ENCODER1_TIM_IRQn);
    }
}

/**
 * @brief 编码器0中断回调
 *
 */
void ENCODER0_TIM_IRQHandler(void) {
    HAL_TIM_IRQHandler(&encoder0_tim_handle);
}

/**
 * @brief 编码器1中断回调
 *
 */
void ENCODER1_TIM_IRQHandler(void) {
    HAL_TIM_IRQHandler(&encoder1_tim_handle);
}
