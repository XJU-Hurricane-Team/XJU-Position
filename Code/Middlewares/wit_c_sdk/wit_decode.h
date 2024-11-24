/**
 * @file    wit_decode.h
 * @author  Deadline039
 * @brief   维特智能传感器数据处理
 * @version 1.0
 * @date    2023-12-02
 */
#ifndef __WIT_DECODE_H
#define __WIT_DECODE_H

#include "bsp.h"

/**
 * @brief 方向定义
 */
enum {
    WIT_ROLL = 0, /*!< roll方向值 */
    WIT_PITCH,    /*!< pitch方向值 */
    WIT_YAW       /*!< yaw方向值 */
};

extern float g_wit_acc[3];
extern float g_wit_gyro[3];
extern float g_wit_angle[3];

void wit_register_uart(UART_HandleTypeDef *huart);

void wit_read_uart(void);

#endif /* __WIT_DECODE_H */
