/**
  ******************************************************************************
  * @file           : tcpsendtask.c
  * @brief          : tcpsendtask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"

/*********TCPSEND_TASK BEGIN***********/
k_task_t tcpsend_task;
k_stack_t stack_tcpsend_task[STK_SIZE_TCPSEND_TASK];

void tcpsend_task_entry(void *arg)
{
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	while(1)
	{
		tos_task_delay(10);
	}
	return;
}
/*********TCPSEND_TASK END***********/
