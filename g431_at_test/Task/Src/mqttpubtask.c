/**
  ******************************************************************************
  * @file           : mqttpubtask.c
  * @brief          : mqttpubtask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"
#include "mqttpub.h"
#include "ntp.h"

/*********MQTTPUB_TASK BEGIN***********/
k_task_t mqttpub_task;
k_stack_t stack_mqttpub_task[STK_SIZE_MQTTPUB_TASK];

void mqttpub_task_entry(void *arg)
{
	k_err_t err;
	k_event_flag_t flag_match;
	
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	//lt:�ȴ����ݷ����¼�-�����ϱ�/�쳣�ϱ�/�����ϱ�
	while(1)
	{
		err = tos_event_pend(&ev_mqttpub, ev_normal_pub | ev_exce_pub | ev_para_pub, &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
		if(err == K_ERR_NONE)
		{
			if(flag_match == ev_normal_pub)
			{
				//lt:������������
				printf("publish normal data\n");
				if(get_unix_time(&g_unixTime.mqtt_unix_et) != 0)
		    {
			    return;
		    }//lt:�ɼ���ɻ��unixʱ��
				mqtt_sensor_pub();
				//lt:��ʱ10msȥ�ٴ�����mqtt�ɼ�����
				tos_task_delay(10);
				if(get_unix_time(&g_unixTime.mqtt_unix_st) != 0)
		    {
			    return;
		    }//lt:�ɼ���ʼ���unixʱ��
				tos_sem_post(&sem_tsample_mqtt);
				tos_sem_post(&sem_nsample_mqtt);
				tos_sem_post(&sem_vsample_mqtt);
			}
			
			if(flag_match == ev_exce_pub)
			{
				//lt:�����쳣����
				//lt:���mqtt�����ӵ�״̬
				printf("publish exce data\n");
				mqtt_alarms_pub();
			}
			
			if(flag_match == ev_para_pub)
			{
				//lt:������������
			}
		}
	}
}

/*********MQTTPUB_TASK END***********/