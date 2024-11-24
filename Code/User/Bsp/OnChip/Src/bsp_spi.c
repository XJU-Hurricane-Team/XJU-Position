
/**
 * @file    bsp_spi.c
 * @author  Deadline039
 * @brief   SPI驱动
 * @version 1.0
 * @date    2024-09-05
 */

#include "bsp_spi.h"

#include <assert.h>

SPI_HandleTypeDef spi_handle;

/**
 * @brief SPI初始化
 *
 */
void spi_init(void) {
    HAL_StatusTypeDef res = HAL_OK;

    SPIx_CLK_ENABLE();
    spi_handle.Instance = SPIx;

    spi_handle.Init.Mode = SPIx_MODE;
    spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
    spi_handle.Init.DataSize = SPI_DATASIZE_16BIT;
    spi_handle.Init.CLKPolarity = SPIx_CLK_POLARITY;
    spi_handle.Init.CLKPhase = SPIx_CLK_PHASE;
    spi_handle.Init.NSS = SPIx_NSS_MODE;
    spi_handle.Init.BaudRatePrescaler = SPIx_CLK_PRESCALE;

    spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
    spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi_handle.Init.CRCPolynomial = 10;

    res = HAL_SPI_Init(&spi_handle);
#ifdef DEBUG
    assert(res == HAL_OK);
#endif /* DEBUG */
}

/**
 * @brief SPI中断服务函数
 *
 */
void SPIx_IRQHandler(void) {
    HAL_SPI_IRQHandler(&spi_handle);
}

/**
 * @brief SPI底层初始化
 *
 * @param hspi SPI句柄
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
    GPIO_InitTypeDef gpio_init_struct;

    if (hspi->Instance == SPIx) {
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;

        if (hspi->Init.NSS == SPI_NSS_HARD_INPUT) {
            gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        } else if (hspi->Init.NSS == SPI_NSS_HARD_OUTPUT) {
            gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        }

        if (hspi->Init.NSS != SPI_NSS_SOFT) {
            SPIx_NSS_GPIO_CLK_ENABLE();
            gpio_init_struct.Pin = SPIx_NSS_GPIO_PIN;
            HAL_GPIO_Init(SPIx_NSS_GPIO_PORT, &gpio_init_struct);
        }

        gpio_init_struct.Mode = GPIO_MODE_AF_PP;

        SPIx_SCK_GPIO_CLK_ENABLE();
        gpio_init_struct.Pin = SPIx_SCK_GPIO_PIN;
        HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &gpio_init_struct);

        SPIx_MOSI_GPIO_CLK_ENABLE();
        gpio_init_struct.Pin = SPIx_MOSI_GPIO_PIN;
        HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &gpio_init_struct);

        SPIx_MISO_GPIO_CLK_ENABLE();
        gpio_init_struct.Pin = SPIx_MISO_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_INPUT;
        gpio_init_struct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &gpio_init_struct);
    }
}
