/**
  ******************************************************************************
  * @file           : vsampletask.c
  * @brief          : vsampletask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"
#include "mqttpub.h"
#include "LIS2DS12_ACC_driver.h"
#include "i2c.h"

/*********VSAMPLE_TASK BEGIN***********/
k_task_t vsample_task;
k_stack_t stack_vsample_task[STK_SIZE_VSAMPLE_TASK];

void vsample_task_entry(void *arg)
{
  k_err_t err;
	k_event_flag_t flag_match;
	//lt:初始化未完成,等待
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	
	while(1)
	{
		err = tos_event_pend(&ev_vsample, ev_rtc_vsample | ev_wake_vsample, &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
		if(err == K_ERR_NONE)
	  {
		  //lt:开始采集震动
		  printf("starat vib sample\n");
			//lt:分配mqttpubx,y,z的空间,每s钟去将缓存数据一次性写入这个空间,然后通知mqttpubtask去发送,在整个采集完成后并写完外部ram后发送信号到tcpsendtask由其释放该空间
			g_sensorTd.pxvibValue = (float*)tos_mmheap_alloc(sizeof(float) * MAX_XYZSAMPLE_CNT);
			g_sensorTd.pyvibValue = (float*)tos_mmheap_alloc(sizeof(float) * MAX_XYZSAMPLE_CNT);
			g_sensorTd.pzvibValue = (float*)tos_mmheap_alloc(sizeof(float) * MAX_XYZSAMPLE_CNT);
			
			while(1) //lt:时间到了退出循环,这个后期根据时间去设置
			{
				//lt:mqtt发送完毕才能再次采集,pend住
				err = tos_sem_pend(&sem_vsample_mqtt, TOS_TIME_FOREVER);
				
				for(int i = 0; i < MAX_XYZSAMPLE_CNT; i++)
			  {
				  int16_t data[3] = {0};
			    LIS2DS12_CFn_IsDeviceReady(&hi2c2);
			    LIS2DS12_CFn_Initialize(&hi2c2);
			    LIS2DS12_CFn_Data_Collection(&hi2c2, data);
				  g_sensorTd.pxvibValue[i] = (float)(data[0]/1000.0);
				  g_sensorTd.pyvibValue[i] = (float)(data[1]/1000.0);
				  g_sensorTd.pzvibValue[i] = (float)(data[2]/1000.0);
			  }
				printf("mqtt temp sample\n");
				//lt:发送mqtt采集完成信号
				tos_task_delay(1000);//lt:后期改全部采样,部分数据进入mqttpub的全局变量,完成mqtt的样本采集后去post
				tos_sem_post(&sem_sample_mqttfin);
			}
	  }
		printf("finish vib sample\n");
		//lt:发送tcp采集完成信号
	}
}
/*********VSAMPLE_TASK END***********/
