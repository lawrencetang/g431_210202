/**
  ******************************************************************************
  * @file           : sram.c
  * @brief          : sram program body
  ******************************************************************************
  */

#include "stm32g4xx_hal.h"
#include "sram.h"

/*
**write one byte
*/
static uint8_t spi_sram_write_byte(SPI_HandleTypeDef *hspi, uint8_t byte)
{
	if(HAL_SPI_Transmit(hspi, &byte, 1, 0xFFFFFF) != HAL_OK)
	{
		return 1;
	}
	return 0;
}

/*
**sram1 read n byte
*/
uint8_t spi_sram1_read_nbyte(uint8_t *pData, uint16_t size, uint32_t addr)
{
	
	SRAM1_CSLOW;
	
	spi_sram_write_byte(&hspi2, SRAM_CMD_READ);
	spi_sram_write_byte(&hspi2, (uint8_t)((addr & 0xFF0000)>>16));
	spi_sram_write_byte(&hspi2, (uint8_t)((addr & 0xFF00)>>8));
	spi_sram_write_byte(&hspi2, (uint8_t)(addr & 0xFF));
	
	if(HAL_SPI_Receive_IT(&hspi2, pData, size) != HAL_OK)
	{
		return 1;
	}
	
	SRAM1_CSHIGH;
	
	return 0;
	
}

/*
**sram2 read n byte
*/
uint8_t spi_sram2_read_nbyte(uint8_t *pData, uint16_t size, uint32_t addr)
{
	
	SRAM2_CSLOW;
	
	spi_sram_write_byte(&hspi3, SRAM_CMD_READ);
	spi_sram_write_byte(&hspi3, (uint8_t)((addr & 0xFF0000)>>16));
	spi_sram_write_byte(&hspi3, (uint8_t)((addr & 0xFF00)>>8));
	spi_sram_write_byte(&hspi3, (uint8_t)(addr & 0xFF));
	
	if(HAL_SPI_Receive_IT(&hspi3, pData, size) != HAL_OK)
	{
		return 1;
	}
	
	SRAM2_CSHIGH;
	
	return 0;
	
}

/*
**sram1 fastread n byte
*/
uint8_t spi_sram1_fastread_nbyte(uint8_t *pData, uint16_t size, uint32_t addr)
{
	
	SRAM1_CSLOW;
	
	spi_sram_write_byte(&hspi2, SRAM_CMD_FASTREAD);
	spi_sram_write_byte(&hspi2, (uint8_t)((addr & 0xFF0000)>>16));
	spi_sram_write_byte(&hspi2, (uint8_t)((addr & 0xFF00)>>8));
	spi_sram_write_byte(&hspi2, (uint8_t)(addr & 0xFF));
	
	spi_sram_write_byte(&hspi2, 0xFF);
	
	if(HAL_SPI_Receive_IT(&hspi2, pData, size) != HAL_OK)
	{
		return 1;
	}
	
	SRAM1_CSHIGH;
	
	return 0;
	
}

/*
**sram2 fastread n byte
*/
uint8_t spi_sram2_fastread_nbyte(uint8_t *pData, uint16_t size, uint32_t addr)
{
	
	SRAM2_CSLOW;
	
	spi_sram_write_byte(&hspi3, SRAM_CMD_FASTREAD);
	spi_sram_write_byte(&hspi3, (uint8_t)((addr & 0xFF0000)>>16));
	spi_sram_write_byte(&hspi3, (uint8_t)((addr & 0xFF00)>>8));
	spi_sram_write_byte(&hspi3, (uint8_t)(addr & 0xFF));
	
	spi_sram_write_byte(&hspi3, 0xFF);
	
	if(HAL_SPI_Receive_IT(&hspi3, pData, size) != HAL_OK)
	{
		return 1;
	}
	
	SRAM2_CSHIGH;
	
	return 0;
	
}

/*
**sram1 write n byte
*/
uint8_t spi_sram1_write_nbyte(uint8_t *pData, uint16_t size, uint32_t addr)
{
	SRAM1_CSLOW;
	
	spi_sram_write_byte(&hspi2, SRAM_CMD_WRITE);
	spi_sram_write_byte(&hspi2, (uint8_t)((addr & 0xFF0000)>>16));
	spi_sram_write_byte(&hspi2, (uint8_t)((addr & 0xFF00)>>8));
	spi_sram_write_byte(&hspi2, (uint8_t)(addr & 0xFF));
	
	if(HAL_SPI_Transmit_IT(&hspi2, pData, size) != HAL_OK)
	{
		return 1;
	}
	
	SRAM1_CSHIGH;
	return 0;
}

/*
**sram2 write n byte
*/
uint8_t spi_sram2_write_nbyte(uint8_t *pData, uint16_t size, uint32_t addr)
{
	SRAM2_CSLOW;
	
	spi_sram_write_byte(&hspi3, SRAM_CMD_WRITE);
	spi_sram_write_byte(&hspi3, (uint8_t)((addr & 0xFF0000)>>16));
	spi_sram_write_byte(&hspi3, (uint8_t)((addr & 0xFF00)>>8));
	spi_sram_write_byte(&hspi3, (uint8_t)(addr & 0xFF));
	
	if(HAL_SPI_Transmit_IT(&hspi3, pData, size) != HAL_OK)
	{
		return 1;
	}
	
	SRAM2_CSHIGH;
	return 0;
}
