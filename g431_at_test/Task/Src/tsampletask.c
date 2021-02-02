/**
  ******************************************************************************
  * @file           : tsampletask.c
  * @brief          : tsampletask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"
#include "mqttpub.h"
#include "STTS75_driver.h"

/*********TSAMPLE_TASK BEGIN***********/
k_task_t tsample_task;
k_stack_t stack_tsample_task[STK_SIZE_TSAMPLE_TASK];

void tsample_task_entry(void *arg)
{
  k_err_t err;
	k_event_flag_t flag_match;
	stts_status_t stat = STTS_ERROR;
	float temperture;
	//lt:初始化未完成,等待
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	while(1)
	{
		err = tos_event_pend(&ev_tsample, ev_rtc_tsample | ev_wake_tsample, &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
		if(err == K_ERR_NONE)
	  {
		  //lt:开始采集温度
		  printf("starat temp sample\n");
			while(1) //lt:时间到了退出循环,这个后期根据时间去设置
			{
				//lt:mqtt发送完毕才能再次采集,pend住
				err = tos_sem_pend(&sem_tsample_mqtt, TOS_TIME_FOREVER);
				
				stat = STTS75_Check(&hi2c1, STTS75_I2C_ADDRESS_READ);
				if(STTS_SUCCESS == stat)
				{
					STTS75_GET_Temp(&hi2c1, &temperture);
				}
				g_sensorTd.tempValue = temperture;
				printf("mqtt temp sample\n");
				//lt:发送mqtt采集完成信号
				tos_task_delay(1000);//lt:后期改全部采样,部分数据进入mqttpub的全局变量,完成mqtt的样本采集后去post
				tos_sem_post(&sem_sample_mqttfin);
			}
			printf("finish temp sample\n");
			//lt:发送tcp采集完成信号
	  }
	}
}
/*********TSAMPLE_TASK END***********/
