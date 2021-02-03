/**
  ******************************************************************************
  * @file           : mqttsubtask.c
  * @brief          : mqttsubtask program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "task.h"
#include "bsp.h"
#include "mqttsub.h"
#include "tcpsend.h"

#define TCP_CONN_URL "10.1.205.57"
#define TCP_CONN_PORT "6666"

/*********MQTTSUB_TASK BEGIN***********/
k_task_t mqttsub_task;
k_stack_t stack_mqttsub_task[STK_SIZE_MQTTSUB_TASK];

void mqttsub_task_entry(void *arg)
{
  k_err_t err;
	char* pmessage_data;
	//lt:初始化未完成,等待
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	//lt:准备连接mqtt
	if(mqtt_conn_fun() != 0)
	{
		printf("can not conn mqtt");
		//lt:不能发信号给ntp任务去设置,设置网络状态
		return;
	}
	
	g_u8MqttOver = MQTT_OVER;
	
	tos_task_delay(1000);
	//lt:设置mqtt连接网络状态,status!!
	//lt:订阅相关消息
	if(mqtt_sub_fun() != 0)
	{
		printf("can not sub mqtt");
		//lt:设置不能订阅的网络状态
		return;
	}
	//lt:设置mqtt订阅网络状态,status!!
	
	//lt:由于内部ram不够,把连接tcp的暂时放在这里
	if(tcp_conn(TCP_CONN_URL, TCP_CONN_PORT) < 0)
	{
		printf("can not conn tcp");
		//lt:释放mqtt连接
		mqtt_clean();
		return;
	}
	
	printf("success conn tcp");
	
	g_u8TcpOver = TCP_OVER;
	
	
	while(1)
	{
		//lt:无处理使能mqtt接收消息
		//lt:阻塞接收mqtt消息
		tos_sem_post(&sem_mqttrev_en);
	  err = tos_sem_pend(&sem_mqttrev_fin, TOS_TIME_FOREVER);
	  if(err == K_ERR_NONE)
	  {
		  //lt:接收到mqtt的消息,开始处理
		  pmessage_data = (char*)g_mqttRevData.pmq_rev_data;
		  printf("my message is %s\n", pmessage_data);
			//lt:解析json格式
			mqtt_rev_json(pmessage_data);
			//lt:处理完成释放堆
			tos_mmheap_free(g_mqttRevData.pmq_rev_data);
			g_mqttRevData.pmq_rev_data = NULL;
	  }
		tos_task_delay(20);
	}
	
	return;
}
/*********MQTTSUB_TASK END***********/
