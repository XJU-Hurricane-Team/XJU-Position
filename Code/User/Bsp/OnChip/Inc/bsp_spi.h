/**
 * @file    bsp_spi.h
 * @author  Deadline039
 * @brief   SPI驱动
 * @version 1.0
 * @date    2024-10-19
 */

#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef spi_handle;

#define SPIx                        SPI1
#define SPIx_CLK_ENABLE()           __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_IRQn                   SPI1_IRQn
#define SPIx_IRQHandler             SPI1_IRQHandler

#define SPIx_DMA_TX_CLK_ENABLE()    __HAL_RCC_DMA1_CLK_ENABLE()
#define SPIx_DMA_TX_CHANNEL         DMA1_Channel3
#define SPIx_DMA_TX_IRQn            DMA1_Channel3_IRQn
#define SPIx_DMA_TX_IRQHandler      DMA1_Channel3_IRQHandler

#define SPIx_DMA_RX_CLK_ENABLE()    __HAL_RCC_DMA1_CLK_ENABLE()
#define SPIx_DMA_RX_CHANNEL         DMA1_Channel2
#define SPIx_DMA_RX_IRQn            DMA1_Channel2_IRQn
#define SPIx_DMA_RX_IRQHandler      DMA1_Channel2_IRQHandler

#define SPIx_NSS_GPIO_PORT          GPIOA
#define SPIx_NSS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_NSS_GPIO_PIN           GPIO_PIN_4

#define SPIx_SCK_GPIO_PORT          GPIOA
#define SPIx_SCK_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_SCK_GPIO_PIN           GPIO_PIN_5

#define SPIx_MISO_GPIO_PORT         GPIOA
#define SPIx_MISO_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_PIN          GPIO_PIN_6

#define SPIx_MOSI_GPIO_PORT         GPIOA
#define SPIx_MOSI_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_PIN          GPIO_PIN_7

// <<< Use Configuration Wizard in Context Menu >>>

// <o SPIx_MODE> SPI模式
//  <SPI_MODE_SLAVE=>从机模式
//  <SPI_MODE_MASTER=>主机模式
#define SPIx_MODE                   SPI_MODE_MASTER

// <o SPIx_NSS_MODE> 片选模式
//  <SPI_NSS_SOFT=>软件片选
//  <SPI_NSS_HARD_INPUT=>硬件输入
//  <SPI_NSS_HARD_OUTPUT=>硬件输出
#define SPIx_NSS_MODE               SPI_NSS_SOFT

// <o SPIx_CLK_POLARITY> 时钟极性
//  <SPI_POLARITY_HIGH=>高电平
//  <SPI_POLARITY_LOW=>低电平
#define SPIx_CLK_POLARITY           SPI_POLARITY_LOW

// <o SPIx_CLK_PHASE> 时钟相位
//  <SPI_PHASE_1EDGE=>0相位
//  <SPI_PHASE_2EDGE=>1相位
#define SPIx_CLK_PHASE              SPI_PHASE_2EDGE

// <o SPIx_CLK_PRESCALE> 分频系数
// <i> 关系到SPI速率
//  <SPI_BAUDRATEPRESCALER_2>2分频
//  <SPI_BAUDRATEPRESCALER_4>4分频
//  <SPI_BAUDRATEPRESCALER_8>8分频
//  <SPI_BAUDRATEPRESCALER_16>16分频
//  <SPI_BAUDRATEPRESCALER_32>32分频
//  <SPI_BAUDRATEPRESCALER_64>64分频
//  <SPI_BAUDRATEPRESCALER_128>128分频
//  <SPI_BAUDRATEPRESCALER_256>256分频
#define SPIx_CLK_PRESCALE           SPI_BAUDRATEPRESCALER_4

// <<< end of configuration section >>>

void spi_init(void);

#endif /* __BSP_SPI_H */