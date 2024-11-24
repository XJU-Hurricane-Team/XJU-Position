/**
 * @file    includes.h
 * @author  Deadline039
 * @brief   Include files
 * @version 1.0
 * @date    2024-04-03
 */

#ifndef __INCLUDES_H
#define __INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Board Support Package Include Files. */
#include "bsp.h"

/* Middlewares Include Files. */
#include "./wit_c_sdk/wit_decode.h"

/* Application Layer Export Types.  */

/**
 * @brief 从串口接收到的数据
 */
typedef enum {
    RECV_DATA_ERROR,        /*!< 接收数据有错误 */
    RECV_DATA_NONE,         /*!< 没有数据 */
    RECV_DATA_OK,           /*!< 成功接收到数据 */
    RECV_DATA_CALI_GYRO,    /*!< 进入陀螺仪校准模式 */
    RECV_DATA_CALI_ENCODER, /*!< 进入正交码盘校准模式 */
} receive_data_t;

/* Application Layer Export variables. */
extern float g_pos_x;
extern float g_pos_y;
extern float g_pos_yaw;
extern float g_pos_yaw_offset;

/* Application Layer Export functions. */
void position_calc(void);

void package_register_uart(UART_HandleTypeDef *huart);
void package_data(void);
receive_data_t package_recv_data(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCLUDES_H */
