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
	//lt:初始化未完成,等待
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	
	//lt:初始化完成,等待读取噪音的事件
	
	while(1)
	{
		err = tos_event_pend(&ev_nsample, ev_rtc_nsample | ev_wake_nsample, &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
		if(err == K_ERR_NONE)
	  {
		  //lt:开始采集噪音
		  printf("starat noise sample\n");
			//lt:分配mqttpubn的空间,每s钟去将缓存数据一次性写入这个空间,然后通知mqttpubtask去发送,在整个采集完成后并写完外部ram后发送信号到tcpsendtask由其释放该空间
			while(1) //lt:时间到了退出循环,这个后期根据时间去设置
			{
				//lt:mqtt发送完毕才能再次采集,pend住
				err = tos_sem_pend(&sem_nsample_mqtt, TOS_TIME_FOREVER);
				
				printf("mqtt noise sample\n");
				//lt:发送mqtt采集完成信号
				tos_task_delay(1000);//lt:后期改全部采样,部分数据进入mqttpub的全局变量,完成mqtt的样本采集后去post
				tos_sem_post(&sem_sample_mqttfin);
			}
	  }
		printf("finish noise sample\n");
		//lt:发送tcp采集完成信号
	}
}
/*********NSAMPLE_TASK END***********/
