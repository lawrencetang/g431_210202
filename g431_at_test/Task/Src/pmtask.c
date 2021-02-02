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
//	//lt:��ʼ��δ���,�ȴ�
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
//			//lt:���Է����쳣����,��ص�����
//			g_alarms.lowPower = 1;
//			tos_event_post(&ev_mqttpub, ev_exce_pub);
//			test_post_exce_cnt = 0;
//		}
//	}
//	//lt:��Ϊ���Գ���
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	while(1)
	{
		tos_task_delay(20000);
		g_alarms.lowPower = 0;
		bat_level = adcGetElectricQuantity();//��õ�ص���
		//���ʹ������͵�����Ӧ��ϵ��Ҫ�����Ĺ����������
    if(4.2 < bat_level && bat_level <= 5.0)
    {
      //���������
    }
    else if(3.2 < bat_level && bat_level <= 4.2)
    {
      //��ص���20%~80%
    }
    else if(bat_level <= 3.2)
    {
      //��ص�����,״̬�ò��ܷ���
			if(g_u8MqttOver == MQTT_OVER)
			{
				g_alarms.lowPower = 1;
				tos_event_post(&ev_mqttpub, ev_exce_pub);
			}
    }
	}

}
/*********PM_TASK END***********/
