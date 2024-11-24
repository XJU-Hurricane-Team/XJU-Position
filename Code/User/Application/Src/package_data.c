/**
 * @file    send_data.c
 * @author  Deadline039
 * @brief   封装数据包, 兼容东大的数据格式
 * @version 1.0
 * @date    2024-09-30
 * @note    仅支持DMA
 */

#include "includes.h"

#include <math.h>
#include <string.h>

#define RECEIVE_PREFIX "ACT"

static UART_HandleTypeDef *uart_handle;

static struct {
    uint8_t head[2]; /*!< 帧头 */
    float yaw;       /*!< 航向角 */
    float pitch;     /*!< 俯仰角 */
    float roll;      /*!< 横滚角 */
    float pos_x;     /*!< 坐标x */
    float pos_y;     /*!< 坐标y */
    float yaw_gyro;  /*!< 航向角速度 */
    uint8_t tail[2]; /*!< 帧尾 */
} __packed send_data;

/**
 * @brief 串口周期发送数据
 *
 * @param htim 定时器句柄
 */
void package_send(TIM_HandleTypeDef *htim) {
    if (htim->Instance != SEND_TIMER) {
        return;
    }

    uart_dmatx_write(uart_handle, &send_data, sizeof(send_data));
    uart_dmatx_send(uart_handle);
}

/**
 * @brief 注册数据发送串口
 *
 * @param huart 串口句柄
 */
void package_register_uart(UART_HandleTypeDef *huart) {
    uart_handle = huart;

    send_data.head[0] = 0x0D;
    send_data.head[1] = 0x0A;
    send_data.tail[0] = 0x0A;
    send_data.tail[1] = 0x0D;

    send_timer_init(package_send);
}

/**
 * @brief 包装数据
 *
 */
void package_data(void) {
    send_data.yaw = g_pos_yaw;
    send_data.pitch = g_wit_angle[WIT_PITCH];
    send_data.roll = g_wit_angle[WIT_ROLL];
    send_data.pos_x = g_pos_x;
    send_data.pos_y = g_pos_y;
    send_data.yaw_gyro = g_wit_gyro[WIT_YAW];
}

/**
 * @brief DMA轮询读取并解析数据
 *
 * @return 收到的数据
 */
receive_data_t package_recv_data(void) {
    static uint8_t uart_buffer[20];

    if (uart_dmarx_read(uart_handle, uart_buffer, sizeof(uart_buffer)) == 0) {
        return RECV_DATA_NONE;
    }

    if (memcmp(uart_buffer, RECEIVE_PREFIX, (sizeof(RECEIVE_PREFIX) - 1))) {
        /* 数据帧格式错误 */
        return RECV_DATA_ERROR;
    }

    float new_yaw_angle;
    uint32_t new_freq;

    switch (uart_buffer[sizeof(RECEIVE_PREFIX) - 1]) {
        case '0': {
            /* 清零, 这里直接重启 */
            HAL_NVIC_SystemReset();
        } break;

        case 'J': {
            /* 设置yaw轴角度 */
            memcpy(&new_yaw_angle, &uart_buffer[4], sizeof(float));

            if (fabsf(new_yaw_angle) > 180.0f) {
                return RECV_DATA_ERROR;
            }
            g_pos_yaw_offset = new_yaw_angle - g_wit_angle[WIT_YAW];
        } break;

        case 'X': {
            /* 设置x轴坐标 */
            memcpy(&g_pos_x, &uart_buffer[4], sizeof(float));
        } break;

        case 'Y': {
            /* 设置y轴坐标 */
            memcpy(&g_pos_y, &uart_buffer[4], sizeof(float));
        } break;

        case 'D': {
            /* 设置x, y轴坐标 */
            memcpy(&g_pos_x, &uart_buffer[4], sizeof(float));
            memcpy(&g_pos_y, &uart_buffer[8], sizeof(float));
        } break;

        case 'A': {
            /* 设置yaw, x, y轴 */
            memcpy(&new_yaw_angle, &uart_buffer[4], sizeof(float));
            memcpy(&g_pos_x, &uart_buffer[8], sizeof(float));
            memcpy(&g_pos_y, &uart_buffer[12], sizeof(float));

            if (fabsf(new_yaw_angle) > 180.0f) {
                return RECV_DATA_ERROR;
            }
            g_pos_yaw_offset = new_yaw_angle - g_wit_angle[WIT_YAW];
        } break;

        case 'F': {
            /* 扩展命令, 设置上传频率, 永久保存 */
            memcpy(&new_freq, &uart_buffer[4], sizeof(uint32_t));
            if ((new_freq < 1) || (new_freq > 200)) {
                return RECV_DATA_ERROR;
            }
            send_timer_set_frequency(new_freq);
        } break;

        case 'R':
            /* 重定义此命令, 收到后转发陀螺仪数据, 供上位机修改校准 */
            return RECV_DATA_CALI_GYRO;

        case 'E':
            /* 扩展命令, 收到后进入正交码盘校准模式 */
            return RECV_DATA_CALI_ENCODER;

        default:
            return RECV_DATA_ERROR;
    }

    return RECV_DATA_OK;
}