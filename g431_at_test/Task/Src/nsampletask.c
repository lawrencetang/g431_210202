/**
  ******************************************************************************
  * @file           : nsampletask.c
  * @brief          : nsampletask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"
#include "mqttpub.h"

/*********NSAMPLE_TASK BEGIN***********/
k_task_t nsample_task;
k_stack_t stack_nsample_task[STK_SIZE_NSAMPLE_TASK];

void nsample_task_entry(void *arg)
{
  k_err_t err;
	k_event_flag_t flag_match;
	//lt:��ʼ��δ���,�ȴ�
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	//lt:��ʼ�����,�ȴ���ȡ�������¼�
	
	while(1)
	{
		err = tos_event_pend(&ev_nsample, ev_rtc_nsample | ev_wake_nsample, &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
		if(err == K_ERR_NONE)
	  {
		  //lt:��ʼ�ɼ�����
		  printf("starat noise sample\n");
			//lt:����mqttpubn�Ŀռ�,ÿs��ȥ����������һ����д������ռ�,Ȼ��֪ͨmqttpubtaskȥ����,�������ɼ���ɺ�д���ⲿram�����źŵ�tcpsendtask�����ͷŸÿռ�
			while(1) //lt:ʱ�䵽���˳�ѭ��,������ڸ���ʱ��ȥ����
			{
				//lt:mqtt������ϲ����ٴβɼ�,pendס
				err = tos_sem_pend(&sem_nsample_mqtt, TOS_TIME_FOREVER);
				
				printf("mqtt noise sample\n");
				//lt:����mqtt�ɼ�����ź�
				tos_task_delay(1000);//lt:���ڸ�ȫ������,�������ݽ���mqttpub��ȫ�ֱ���,���mqtt�������ɼ���ȥpost
				tos_sem_post(&sem_sample_mqttfin);
			}
	  }
		printf("finish noise sample\n");
		//lt:����tcp�ɼ�����ź�
	}
}
/*********NSAMPLE_TASK END***********/
