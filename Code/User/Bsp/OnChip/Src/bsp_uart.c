/**
 * @file    uart.c
 * @author  Deadline039
 * @brief   STM32F1 串口驱动
 * @version 1.2
 * @date    2023-09-14
 */

#include "bsp_uart.h"

#include "core_delay.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char uart_buffer[256];

extern void uart_dmarx_idle_callback(UART_HandleTypeDef *huart);

#if (USART1_ENABLE == 1)

UART_HandleTypeDef usart1_handle;

/**
 * @brief 串口1初始化
 * 
 * @param baud_rate 波特率
 */
void usart1_init(uint32_t baud_rate) {
    GPIO_InitTypeDef gpio_init_struct;
    HAL_StatusTypeDef res = HAL_OK;

    usart1_handle.Instance = USART1;

    usart1_handle.Init.BaudRate = baud_rate;
    usart1_handle.Init.WordLength = UART_WORDLENGTH_8B;
    usart1_handle.Init.StopBits = UART_STOPBITS_1;
    usart1_handle.Init.Parity = UART_PARITY_NONE;
    usart1_handle.Init.HwFlowCtl = USART1_HWCONTROL;
    usart1_handle.Init.Mode = UART_MODE_TX_RX;

    __HAL_RCC_USART1_CLK_ENABLE();

    res = HAL_UART_Init(&usart1_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (usart1_handle.Init.Mode & UART_MODE_TX) {
        USART1_TX_GPIO_ENABLE();
        gpio_init_struct.Pin = USART1_TX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &gpio_init_struct);
    }

    if (usart1_handle.Init.Mode & UART_MODE_RX) {
        USART1_RX_GPIO_ENABLE();
        gpio_init_struct.Pin = USART1_RX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &gpio_init_struct);
    }

    if (usart1_handle.Init.HwFlowCtl & UART_HWCONTROL_RTS) {
        USART1_RTS_GPIO_ENABLE();
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pin = USART1_RTS_GPIO_PIN;
        HAL_GPIO_Init(USART1_RTS_GPIO_PORT, &gpio_init_struct);
    }

    if (usart1_handle.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
        USART1_CTS_GPIO_ENABLE();
        gpio_init_struct.Pin = USART1_CTS_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART1_CTS_GPIO_PORT, &gpio_init_struct);
    }

    HAL_NVIC_EnableIRQ(USART1_IRQn);
    HAL_NVIC_SetPriority(USART1_IRQn, USART1_IT_PREEMPT, USART1_IT_SUB);

#if (USART1_USE_DMA_TX == 1)
    extern void usart1_dmatx_init(void);
    usart1_dmatx_init();
#endif /* USART1_USE_DMA_TX == 1 */

#if (USART1_USE_DMA_RX == 1)
    extern void usart1_dmarx_init(void);
    usart1_dmarx_init();
#endif /* USART1_USE_DMA_RX == 1 */
}

/**
 * @brief 串口1中断服务函数
 */
void USART1_IRQHandler(void) {

#if (USART1_USE_IDLE_IT == 1)
    if (__HAL_UART_GET_FLAG(&usart1_handle, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&usart1_handle);
        uart_dmarx_idle_callback(&usart1_handle);
    }
#endif /* USART1_USE_IDLE_IT == 1 */

    HAL_UART_IRQHandler(&usart1_handle);
}

#endif /* USART1_ENABLE == 1 */

#if (USART2_ENABLE == 1)

UART_HandleTypeDef usart2_handle;

/**
 * @brief 串口2初始化
 * 
 * @param baud_rate 波特率
 */
void usart2_init(uint32_t baud_rate) {
    GPIO_InitTypeDef gpio_init_struct;
    HAL_StatusTypeDef res = HAL_OK;

    usart2_handle.Instance = USART2;

    usart2_handle.Init.BaudRate = baud_rate;
    usart2_handle.Init.WordLength = UART_WORDLENGTH_8B;
    usart2_handle.Init.StopBits = UART_STOPBITS_1;
    usart2_handle.Init.Parity = UART_PARITY_NONE;
    usart2_handle.Init.HwFlowCtl = USART2_HWCONTROL;
    usart2_handle.Init.Mode = UART_MODE_TX_RX;

    __HAL_RCC_USART2_CLK_ENABLE();

    res = HAL_UART_Init(&usart2_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (usart2_handle.Init.Mode & UART_MODE_TX) {
        USART2_TX_GPIO_ENABLE();
        gpio_init_struct.Pin = USART2_TX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &gpio_init_struct);
    }

    if (usart2_handle.Init.Mode & UART_MODE_RX) {
        USART2_RX_GPIO_ENABLE();
        gpio_init_struct.Pin = USART2_RX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &gpio_init_struct);
    }

    if (usart2_handle.Init.HwFlowCtl & UART_HWCONTROL_RTS) {
        USART2_RTS_GPIO_ENABLE();
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pin = USART2_RTS_GPIO_PIN;
        HAL_GPIO_Init(USART2_RTS_GPIO_PORT, &gpio_init_struct);
    }

    if (usart2_handle.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
        USART2_CTS_GPIO_ENABLE();
        gpio_init_struct.Pin = USART2_CTS_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART2_CTS_GPIO_PORT, &gpio_init_struct);
    }

    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, USART2_IT_PREEMPT, USART2_IT_SUB);

#if (USART2_USE_DMA_TX == 1)
    extern void usart2_dmatx_init(void);
    usart2_dmatx_init();
#endif /* USART2_USE_DMA_TX == 1 */

#if (USART2_USE_DMA_RX == 1)
    extern void usart2_dmarx_init(void);
    usart2_dmarx_init();
#endif /* USART2_USE_DMA_RX == 1 */
}

/**
 * @brief 串口2中断服务函数
 */
void USART2_IRQHandler(void) {

#if (USART2_USE_IDLE_IT == 1)
    if (__HAL_UART_GET_FLAG(&usart2_handle, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&usart2_handle);
        uart_dmarx_idle_callback(&usart2_handle);
    }
#endif /* USART2_USE_IDLE_IT == 1 */

    HAL_UART_IRQHandler(&usart2_handle);
}

#endif /* USART2_ENABLE == 1 */

#if (USART3_ENABLE == 1)

UART_HandleTypeDef usart3_handle;

/**
 * @brief 串口3初始化
 * 
 * @param baud_rate 波特率
 */
void usart3_init(uint32_t baud_rate) {
    GPIO_InitTypeDef gpio_init_struct;
    HAL_StatusTypeDef res = HAL_OK;

    usart3_handle.Instance = USART3;

    usart3_handle.Init.BaudRate = baud_rate;
    usart3_handle.Init.WordLength = UART_WORDLENGTH_8B;
    usart3_handle.Init.StopBits = UART_STOPBITS_1;
    usart3_handle.Init.Parity = UART_PARITY_NONE;
    usart3_handle.Init.HwFlowCtl = USART3_HWCONTROL;
    usart3_handle.Init.Mode = UART_MODE_TX_RX;

    __HAL_RCC_USART3_CLK_ENABLE();

    res = HAL_UART_Init(&usart3_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (usart3_handle.Init.Mode & UART_MODE_TX) {
        USART3_TX_GPIO_ENABLE();
        gpio_init_struct.Pin = USART3_TX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(USART3_TX_GPIO_PORT, &gpio_init_struct);
    }

    if (usart3_handle.Init.Mode & UART_MODE_RX) {
        USART3_RX_GPIO_ENABLE();
        gpio_init_struct.Pin = USART3_RX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART3_RX_GPIO_PORT, &gpio_init_struct);
    }

    if (usart3_handle.Init.HwFlowCtl & UART_HWCONTROL_RTS) {
        USART3_RTS_GPIO_ENABLE();
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pin = USART3_RTS_GPIO_PIN;
        HAL_GPIO_Init(USART3_RTS_GPIO_PORT, &gpio_init_struct);
    }

    if (usart3_handle.Init.HwFlowCtl & UART_HWCONTROL_CTS) {
        USART3_CTS_GPIO_ENABLE();
        gpio_init_struct.Pin = USART3_CTS_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(USART3_CTS_GPIO_PORT, &gpio_init_struct);
    }

    HAL_NVIC_EnableIRQ(USART3_IRQn);
    HAL_NVIC_SetPriority(USART3_IRQn, USART3_IT_PREEMPT, USART3_IT_SUB);

#if (USART3_USE_DMA_TX == 1)
    extern void usart3_dmatx_init(void);
    usart3_dmatx_init();
#endif /* USART3_USE_DMA_TX == 1 */

#if (USART3_USE_DMA_RX == 1)
    extern void usart3_dmarx_init(void);
    usart3_dmarx_init();
#endif /* USART3_USE_DMA_RX == 1 */
}

/**
 * @brief 串口3中断服务函数
 */
void USART3_IRQHandler(void) {

#if (USART3_USE_IDLE_IT == 1)
    if (__HAL_UART_GET_FLAG(&usart3_handle, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&usart3_handle);
        uart_dmarx_idle_callback(&usart3_handle);
    }
#endif /* USART3_USE_IDLE_IT == 1 */

    HAL_UART_IRQHandler(&usart3_handle);
}

#endif /* USART3_ENABLE == 1 */

#if (UART4_ENABLE == 1)

UART_HandleTypeDef uart4_handle;

/**
 * @brief 串口4初始化
 * 
 * @param baud_rate 波特率
 */
void uart4_init(uint32_t baud_rate) {
    GPIO_InitTypeDef gpio_init_struct;
    HAL_StatusTypeDef res = HAL_OK;

    uart4_handle.Instance = UART4;

    uart4_handle.Init.BaudRate = baud_rate;
    uart4_handle.Init.WordLength = UART_WORDLENGTH_8B;
    uart4_handle.Init.StopBits = UART_STOPBITS_1;
    uart4_handle.Init.Parity = UART_PARITY_NONE;
    uart4_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart4_handle.Init.Mode = UART_MODE_TX_RX;

    __HAL_RCC_UART4_CLK_ENABLE();

    res = HAL_UART_Init(&uart4_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (uart4_handle.Init.Mode & UART_MODE_TX) {
        UART4_TX_GPIO_ENABLE();
        gpio_init_struct.Pin = UART4_TX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(UART4_TX_GPIO_PORT, &gpio_init_struct);
    }

    if (uart4_handle.Init.Mode & UART_MODE_RX) {
        UART4_RX_GPIO_ENABLE();
        gpio_init_struct.Pin = UART4_RX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(UART4_RX_GPIO_PORT, &gpio_init_struct);
    }

    HAL_NVIC_EnableIRQ(UART4_IRQn);
    HAL_NVIC_SetPriority(UART4_IRQn, UART4_IT_PREEMPT, UART4_IT_SUB);

#if (UART4_USE_DMA_TX == 1)
    extern void uart4_dmatx_init(void);
    uart4_dmatx_init();
#endif /* UART4_USE_DMA_TX == 1 */

#if (UART4_USE_DMA_RX == 1)
    extern void uart4_dmarx_init(void);
    uart4_dmarx_init();
#endif /* UART4_USE_DMA_RX == 1 */
}

/**
 * @brief 串口4中断服务函数
 */
void UART4_IRQHandler(void) {

#if (UART4_USE_IDLE_IT == 1)
    if (__HAL_UART_GET_FLAG(&uart4_handle, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&uart4_handle);
        uart_dmarx_idle_callback(&uart4_handle);
    }
#endif /* UART4_USE_IDLE_IT == 1 */

    HAL_UART_IRQHandler(&uart4_handle);
}

#endif /* UART4_ENABLE == 1 */

#if (UART5_ENABLE == 1)

UART_HandleTypeDef uart5_handle;

/**
 * @brief 串口5初始化
 * 
 * @param baud_rate 波特率
 */
void uart5_init(uint32_t baud_rate) {
    GPIO_InitTypeDef gpio_init_struct;
    HAL_StatusTypeDef res = HAL_OK;

    uart5_handle.Instance = UART5;

    uart5_handle.Init.BaudRate = baud_rate;
    uart5_handle.Init.WordLength = UART_WORDLENGTH_8B;
    uart5_handle.Init.StopBits = UART_STOPBITS_1;
    uart5_handle.Init.Parity = UART_PARITY_NONE;
    uart5_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart5_handle.Init.Mode = UART_MODE_TX_RX;

    __HAL_RCC_UART5_CLK_ENABLE();

    res = HAL_UART_Init(&uart5_handle);

#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */

    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (uart5_handle.Init.Mode & UART_MODE_TX) {
        UART5_TX_GPIO_ENABLE();
        gpio_init_struct.Pin = UART5_TX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(UART5_TX_GPIO_PORT, &gpio_init_struct);
    }

    if (uart5_handle.Init.Mode & UART_MODE_RX) {
        UART5_RX_GPIO_ENABLE();
        gpio_init_struct.Pin = UART5_RX_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(UART5_RX_GPIO_PORT, &gpio_init_struct);
    }

    HAL_NVIC_EnableIRQ(UART5_IRQn);
    HAL_NVIC_SetPriority(UART5_IRQn, UART5_IT_PREEMPT, UART5_IT_SUB);
}

/**
 * @brief 串口5中断服务函数
 */
void UART5_IRQHandler(void) {
    HAL_UART_IRQHandler(&uart5_handle);
}

#endif /* UART5_ENABLE == 1 */

/**
 * @brief 串口格式化输出
 *
 * @param huart 串口句柄
 * @param __format 格式字符串
 * @return 写入的字符串长度(不包括'\0')
 */
int uart_printf(UART_HandleTypeDef *huart, const char *__format, ...) {
    int res;
    uint16_t len;
    va_list ap;

    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_TC) == RESET) {
        /* 等待发送完毕 */
        delay_ms(1);
    }

    va_start(ap, __format);
    res = vsnprintf(uart_buffer, sizeof(uart_buffer), __format, ap);
    va_end(ap);

    len = strlen(uart_buffer);

    if (huart->hdmatx != NULL) {
        HAL_UART_Transmit_DMA(huart, (uint8_t *)uart_buffer, len);
    } else {
        HAL_UART_Transmit(huart, (uint8_t *)uart_buffer, len, 1000);
    }

    return res;
}

/**
 * @brief 等待串口接收到数据并填充
 *
 * @param huart 串口句柄
 * @param __format 格式字符串
 * @return 成功填充的变量个数
 */
int uart_scanf(UART_HandleTypeDef *huart, const char *__format, ...) {
    uint16_t str_len = 0;
    int res;
    va_list ap;

    if (huart->hdmarx != NULL) {
        while (str_len == 0) {
            str_len = uart_dmarx_read(huart, uart_buffer, sizeof(uart_buffer));
            delay_ms(1);
        }
    } else {
        HAL_UARTEx_ReceiveToIdle(huart, (uint8_t *)uart_buffer,
                                 sizeof(uart_buffer), &str_len, 0xFFFF);
    }

    va_start(ap, __format);
    res = vsscanf((char *)uart_buffer, __format, ap);
    va_end(ap);

    return res;
}

/**
 * @brief 串口错误回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    __IO uint32_t error_code = 0x00U;

    error_code = HAL_UART_GetError(huart);
    if (HAL_UART_ERROR_NONE == error_code) {
        return;
    }

    switch (error_code) {
        case HAL_UART_ERROR_PE: {
            __HAL_UART_CLEAR_PEFLAG(huart);
        } break;

        case HAL_UART_ERROR_NE: {
            __HAL_UART_CLEAR_NEFLAG(huart);
        } break;

        case HAL_UART_ERROR_FE: {
            __HAL_UART_CLEAR_FEFLAG(huart);
        } break;

        case HAL_UART_ERROR_ORE: {
            __HAL_UART_CLEAR_OREFLAG(huart);
        } break;

        case HAL_UART_ERROR_DMA: {
        } break;

        default: {
        } break;
    }

    if (NULL != huart->hdmarx) {
        while (
            HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr, huart->RxXferSize)) {
            __HAL_UNLOCK(huart);
        }
    } else {
        /* 恢复接收地址指针到初始 buffer 位置 ，初始地址 = 当前地址 -
         * 已接收的数据个数，已接收的数据个数 = 需要接收数 - 还未接收数*/
        while (HAL_UART_Receive_IT(
            huart, huart->pRxBuffPtr - (huart->RxXferSize - huart->RxXferCount),
            huart->RxXferSize)) {
            __HAL_UNLOCK(huart);
        }
    }
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)

extern void uart_dmatx_clear_tc_flag(UART_HandleTypeDef *huart);

extern void uart_dmarx_halfdone_callback(UART_HandleTypeDef *huart);
extern void uart_dmarx_done_callback(UART_HandleTypeDef *huart);

/**
 * @brief 串口发送完成回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->hdmatx != NULL) {
        uart_dmatx_clear_tc_flag(huart);
    }
}

/**
 * @brief 串口接收完成回调
 *
 * @param huart 串口句柄
 * @note 如果要多个串口使用接收回调, 强烈建议使用函数指针回调
 *       否则该函数结构会变得非常复杂
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->hdmarx != NULL) {
        uart_dmarx_done_callback(huart);
    }
}

/**
 * @brief 串口接收半满回调
 *
 * @param huart 串口句柄
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->hdmarx != NULL) {
        uart_dmarx_halfdone_callback(huart);
    }
}

#endif /* USE_HAL_UART_REGISTER_CALLBACKS == 0 */
