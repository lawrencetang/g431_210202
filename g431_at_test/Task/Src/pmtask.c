/**
  ******************************************************************************
  * @file           : pmtask.c
  * @brief          : pmtask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"
#include "mqttpub.h"
#include "battery.h"

/*********PM_TASK BEGIN***********/
k_task_t pm_task;
k_stack_t stack_pm_task[STK_SIZE_PM_TASK];

void pm_task_entry(void *arg)
{
	float bat_level;
//	int test_post_exce_cnt = 0;
//	//lt:初始化未完成,等待
//	while(g_u8StartOver != START_OVER)
//	{
//		tos_task_delay(20);
//	}
//	
//	tos_task_delay(8000);//lttest
//	while(1)
//	{
//		tos_task_delay(20);
//		test_post_exce_cnt++;
//		if(test_post_exce_cnt > 400)
//		{
//			//lt:测试发送异常数据,电池电量低
//			g_alarms.lowPower = 1;
//			tos_event_post(&ev_mqttpub, ev_exce_pub);
//			test_post_exce_cnt = 0;
//		}
//	}
//	//lt:均为测试程序
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	while(1)
	{
		tos_task_delay(20000);
		g_alarms.lowPower = 0;
		bat_level = adcGetElectricQuantity();//获得电池电量
		//电池使用情况和电量对应关系需要完整的过程试验测试
    if(4.2 < bat_level && bat_level <= 5.0)
    {
      //电池满电量
    }
    else if(3.2 < bat_level && bat_level <= 4.2)
    {
      //电池电量20%~80%
    }
    else if(bat_level <= 3.2)
    {
      //电池电量低,状态好才能发送
			if(g_u8MqttOver == MQTT_OVER)
			{
				g_alarms.lowPower = 1;
				tos_event_post(&ev_mqttpub, ev_exce_pub);
			}
    }
	}

}
/*********PM_TASK END***********/
