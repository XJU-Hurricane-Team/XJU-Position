/**
 * @file    main.c
 * @author  Deadline039
 * @brief   主函数
 * @version 1.0
 * @date    2024-07-29
 */

#include "includes.h"

#include <inttypes.h>
#include <string.h>

void enter_cali_gyro(void);
void enter_cali_encoder(void);

#include "arm_math.h"
#include "math.h"

/**
 * @brief 主函数
 *
 * @return 退出码
 */
int main(void) {
    receive_data_t recv_data = RECV_DATA_NONE;

    bsp_init();

    wit_register_uart(&usart2_handle);
    package_register_uart(&usart1_handle);

    /* 等待直至收到陀螺仪数据 */
    while (*(int *)(&g_wit_angle[WIT_YAW]) == 0) {
        wit_read_uart();
    }
    g_pos_yaw_offset = -g_wit_angle[WIT_YAW];
    sample_timer_init(5);

    while (1) {
        wit_read_uart();
        if (g_position_calc_flag) {
            position_calc();
            g_position_calc_flag = 0;
        }
        package_data();

        recv_data = package_recv_data();
        switch (recv_data) {
            case RECV_DATA_CALI_GYRO: {
                enter_cali_gyro();
            } break;
            case RECV_DATA_CALI_ENCODER: {
                enter_cali_encoder();
            } break;

            default: {
            } break;
        }

        /* 每 500 ms 闪烁一次 */
        if ((HAL_GetTick() % 1024) > 512) {
            LED_RED_ON();
        } else {
            LED_RED_OFF();
        }
    }
}

/**
 * @brief 进入陀螺仪校准模式
 *
 */
void enter_cali_gyro(void) {
    LED_RED_OFF();
    LED_GREEN_ON();
    uint32_t pc_uart_len = 0;
    uint8_t pc_uart_buf[20];

    uint32_t wit_uart_len = 0;
    uint8_t wit_uart_buf[20];
    usart1_handle.Init.BaudRate = 9600;
    send_timer_disable();
    HAL_UART_Init(&usart1_handle);

    while (1) {
        uart_dmatx_send(&usart1_handle);
        wit_uart_len =
            uart_dmarx_read(&usart2_handle, wit_uart_buf, sizeof(wit_uart_buf));
        if (wit_uart_len != 0) {
            uart_dmatx_write(&usart1_handle, wit_uart_buf, wit_uart_len);
        }

        uart_dmatx_send(&usart2_handle);
        pc_uart_len =
            uart_dmarx_read(&usart1_handle, pc_uart_buf, sizeof(pc_uart_buf));
        if (pc_uart_len != 0) {
            uart_dmatx_write(&usart2_handle, pc_uart_buf, pc_uart_len);
        }
    }
}

/**
 * @brief 进入正交码盘校准模式
 *
 */
void enter_cali_encoder(void) {
    char send_buf[50];
    char uart_recv_buf[10];
    LED_RED_ON();
    send_timer_disable();
    HAL_UART_DeInit(&usart2_handle);

    as5047_reset(0);
    as5047_reset(1);

    uint32_t times = 0;
    int str_len = 0;
    while (1) {
        if (times == 10) {
            str_len = sprintf(send_buf, "%" PRId64 ",%" PRId64 "\n",
                              g_as5047_position[0].total_position,
                              g_as5047_position[1].total_position);

            uart_dmatx_write(&usart1_handle, send_buf, str_len);
            uart_dmatx_send(&usart1_handle);
            times = 0;
        }

        as5047_get_relative_position(0);
        send_buf[0] = g_as5047_position[0].total_position;
        as5047_get_relative_position(1);
        send_buf[1] = g_as5047_position[1].total_position;

        if (uart_dmarx_read(&usart1_handle, uart_recv_buf,
                            sizeof(uart_recv_buf))) {
            if (strcmp(uart_recv_buf, "RESET") == 0) {
                as5047_reset(0);
                as5047_reset(1);
            }
        }

        delay_ms(1);
        ++times;
    }
}
