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
	//lt:��ʼ��δ���,�ȴ�
	while(g_u8StartOver != START_OVER)
	{
		tos_task_delay(20);
	}
	//lt:׼������mqtt
	if(mqtt_conn_fun() != 0)
	{
		printf("can not conn mqtt");
		//lt:���ܷ��źŸ�ntp����ȥ����,��������״̬
		return;
	}
	
	g_u8MqttOver = MQTT_OVER;
	
	tos_task_delay(1000);
	//lt:����mqtt��������״̬,status!!
	//lt:���������Ϣ
	if(mqtt_sub_fun() != 0)
	{
		printf("can not sub mqtt");
		//lt:���ò��ܶ��ĵ�����״̬
		return;
	}
	//lt:����mqtt��������״̬,status!!
	
	//lt:�����ڲ�ram����,������tcp����ʱ��������
	if(tcp_conn(TCP_CONN_URL, TCP_CONN_PORT) < 0)
	{
		printf("can not conn tcp");
		//lt:�ͷ�mqtt����
		mqtt_clean();
		return;
	}
	
	printf("success conn tcp");
	
	g_u8TcpOver = TCP_OVER;
	
	
	while(1)
	{
		//lt:�޴���ʹ��mqtt������Ϣ
		//lt:��������mqtt��Ϣ
		tos_sem_post(&sem_mqttrev_en);
	  err = tos_sem_pend(&sem_mqttrev_fin, TOS_TIME_FOREVER);
	  if(err == K_ERR_NONE)
	  {
		  //lt:���յ�mqtt����Ϣ,��ʼ����
		  pmessage_data = (char*)g_mqttRevData.pmq_rev_data;
		  printf("my message is %s\n", pmessage_data);
			//lt:����json��ʽ
			mqtt_rev_json(pmessage_data);
			//lt:��������ͷŶ�
			tos_mmheap_free(g_mqttRevData.pmq_rev_data);
			g_mqttRevData.pmq_rev_data = NULL;
	  }
		tos_task_delay(20);
	}
	
	return;
}
/*********MQTTSUB_TASK END***********/
