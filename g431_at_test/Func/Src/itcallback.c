/**
  ******************************************************************************
  * @file           : itcallback.h
  * @brief          : Header for itcallback.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* USER CODE BEGIN Includes */
#include "stm32g4xx_hal.h"
#include "main.h"
#include "spi.h"
#include "usart.h"

#include "tos_at.h"
#include "itcallback.h"
#include "include.h"
#include "task.h"
#include "ntp.h"
#include "bsp.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
uint8_t uart1recv_data;
uint8_t uart3recv_data;
uint8_t uart3cach_data[512] = {0};
uint8_t uart3itcount = 0;
/* USER CODE END PV */

/*
** spi tx call back
*/
void HAL_SPI_TxCpltCallback (SPI_HandleTypeDef * hspi)
{
}

/*
** spi rx call back
*/
void HAL_SPI_RxCpltCallback (SPI_HandleTypeDef * hspi)
{
}

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @note   This example shows a simple way to report end of IT Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART1)
	{
		HAL_UART_Receive_IT(&huart1, (uint8_t*)&uart1recv_data, 1);
		tos_at_uart_input_byte(uart1recv_data);
	}
	else if(UartHandle->Instance == USART3)
	{
		//memcpy(&uart3cach_data[uart3itcount++], uart3recv_data, 1);
		
		if(HAL_UART_Receive_IT(&huart3, (uint8_t*)&uart3recv_data, 1) != HAL_OK)
		{
			//printf("HAL_UART_Receive_IT error");
		}
		uart3cach_data[uart3itcount++] = uart3recv_data;
		if(uart3recv_data == '\n')
		{
			tos_sem_post(&uart3_rec_sem);
			uart3itcount = 0;
		}
		
	}
}

/**
  * @brief  Alarm callback
  * @param  hrtc : RTC handle
  * @retval None
  */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	//lt:发信号的时候需要检测状态,各种状态好才能发信号,包括判断不能是持续采集和唤醒采集才能发这几个信号
	//lt:重新设定RTC时间,这里以后可以改进为一直使用RTC来做时间处理
	if(g_u8MqttOver == MQTT_OVER)
	{
		tos_event_post(&ev_tsample, ev_rtc_tsample);
	  tos_event_post(&ev_nsample, ev_rtc_nsample);
	  tos_event_post(&ev_vsample, ev_rtc_vsample);
		
		if(get_unix_time(&g_unixTime.mqtt_unix_st) != 0)
		{
			return;
		}
		tos_sem_post(&sem_tsample_mqtt);
		tos_sem_post(&sem_nsample_mqtt);
		tos_sem_post(&sem_vsample_mqtt);
	}
}
