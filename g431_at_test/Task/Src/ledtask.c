/**
  ******************************************************************************
  * @file           : ledtask.c
  * @brief          : ledtask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "main.h"

/*********PM_TASK BEGIN***********/
k_task_t led_task;
k_stack_t stack_led_task[STK_SIZE_LED_TASK];

void led_task_entry(void *arg)
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);//lt:暂时调试使用
		tos_task_delay(2000);
	}
	
	return;
}
/*********PM_TASK END***********/
