/**
 * @file    wit_decode.c
 * @author  Deadline039
 * @brief   维特智能传感器数据处理
 * @version 1.0
 * @date    2023-12-02
 */

#include "wit_decode.h"

#include "./wit_c_sdk/REG.h"
#include "./wit_c_sdk/wit_c_sdk.h"

/* 加速度 */
float g_wit_acc[3];
/* 陀螺仪 */
float g_wit_gyro[3];
/* 角度 */
float g_wit_angle[3];

#define ACC_UPDATE   0x01
#define GYRO_UPDATE  0x02
#define ANGLE_UPDATE 0x04
#define MAG_UPDATE   0x08
#define READ_UPDATE  0x80

/* 数据更新标志 */
static uint8_t g_DataUpdate = 0;

/**
 * @brief 更新传感器值
 *
 * @param reg 寄存器地址
 * @param reg_len 寄存器计数
 */
static void wit_reg_update(uint32_t reg, uint32_t reg_len) {
    for (uint8_t i = 0; i < reg_len; i++) {
        switch (reg) {
            case AZ:
                g_DataUpdate |= ACC_UPDATE;
                break;
            case GZ:
                g_DataUpdate |= GYRO_UPDATE;
                break;
            case HZ:
                g_DataUpdate |= MAG_UPDATE;
                break;
            case Yaw:
                g_DataUpdate |= ANGLE_UPDATE;
                break;
            default:
                g_DataUpdate |= READ_UPDATE;
                break;
        }
        reg++;
    }
}

static UART_HandleTypeDef *uart_handle;

/**
 * @brief 串口发送函数
 *
 * @param data 数据
 * @param len 数据长度
 */
static void uart_send(uint8_t *data, uint32_t len) {
    if (uart_handle->hdmatx == NULL) {
        HAL_UART_Transmit(uart_handle, data, len, 1000);
    } else {
        HAL_UART_Transmit_DMA(uart_handle, data, len);
    }
}

/**
 * @brief 按字节解析数据
 *
 * @param byte 字节
 */
static void wit_decode(uint8_t byte) {
    WitSerialDataIn(byte);
    if (g_DataUpdate) {
        for (uint8_t i = 0; i < 3; i++) {
            g_wit_acc[i] = sReg[AX + i] / 32768.0l * 16.0;
            g_wit_gyro[i] = sReg[GX + i] / 32768.0l * 2000.0l;
            g_wit_angle[i] = sReg[Roll + i] / 32768.0l * 180.0l;
        }
        if (g_DataUpdate & ACC_UPDATE) {
            g_DataUpdate &= ~ACC_UPDATE;
        }
        if (g_DataUpdate & GYRO_UPDATE) {
            g_DataUpdate &= ~GYRO_UPDATE;
        }
        if (g_DataUpdate & ANGLE_UPDATE) {
            g_DataUpdate &= ~ANGLE_UPDATE;
        }
        if (g_DataUpdate & MAG_UPDATE) {
            g_DataUpdate &= ~MAG_UPDATE;
        }
    }
}

/**
 * @brief wit注册串口(DMA方式)
 *
 * @param huart 串口句柄
 */
void wit_register_uart(UART_HandleTypeDef *huart) {
    uart_handle = huart;

    WitInit(WIT_PROTOCOL_NORMAL, 0x50);

    WitSerialWriteRegister(uart_send);
    WitRegisterCallBack(wit_reg_update);
    WitDelayMsRegister(delay_ms);
}

/**
 * @brief DMA轮询读取串口数据
 *
 * @note 需要先启用DMA, 否则什么都接收不到
 */
void wit_read_uart(void) {
    static uint8_t buf[20];
    uint32_t len = uart_dmarx_read(uart_handle, buf, sizeof(buf));

    if (len == 0) {
        return;
    }

    for (size_t i = 0; i < len; i++) {
        wit_decode(buf[i]);
    }
}
