/**
  ******************************************************************************
  * @file           : sram.h
  * @brief          : Header for sram.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
#ifndef _SRAM_H
#define _SRAM_H

#include "stm32g4xx_hal.h"
#include "spi.h"

#define SRAM_CMD_FASTREAD 0x0b
#define SRAM_CMD_READ 0x03
#define SRAM_CMD_WRITE 0x02
#define SRAM_CMD_RESET 0x99

#define SRAM1_CSLOW HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET)
#define SRAM2_CSLOW HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET)
#define SRAM1_CSHIGH HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET)
#define SRAM2_CSHIGH HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET)

extern uint8_t spi_sram1_read_nbyte(uint8_t *pData, uint16_t size, uint32_t addr);
extern uint8_t spi_sram2_read_nbyte(uint8_t *pData, uint16_t size, uint32_t addr);
extern uint8_t spi_sram1_fastread_nbyte(uint8_t *pData, uint16_t size, uint32_t addr);
extern uint8_t spi_sram2_fastread_nbyte(uint8_t *pData, uint16_t size, uint32_t addr);
extern uint8_t spi_sram1_write_nbyte(uint8_t *pData, uint16_t size, uint32_t addr);
extern uint8_t spi_sram2_write_nbyte(uint8_t *pData, uint16_t size, uint32_t addr);

#endif
