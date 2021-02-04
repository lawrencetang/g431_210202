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
#include "tcpsend.h"

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
	
	//lt:等待数据发送事件-正常上报/异常上报/参数上报
	while(1)
	{
		err = tos_event_pend(&ev_mqttpub, ev_normal_pub | ev_exce_pub | ev_para_pub, &flag_match, TOS_TIME_FOREVER, TOS_OPT_EVENT_PEND_ANY | TOS_OPT_EVENT_PEND_CLR);
		if(err == K_ERR_NONE)
		{
			if(flag_match == ev_normal_pub)
			{
				if(g_ttcpfin_flag > TCPFIN_SAMPLECNT && g_ntcpfin_flag > TCPFIN_SAMPLECNT && g_vtcpfin_flag > TCPFIN_SAMPLECNT)
				{
					//lt:先发布mqtt数据,再发布tcp数据,发完清空相关buffer,break,等待下次RTC或者唤醒
					//printf("publish normal data\n");
					if(get_unix_time(&g_unixTime.mqtt_unix_et) != 0)
		      {
			      return;
		      }//lt:采集完成获得unix时间
					//printf("ready send tcp data");
					//lt:发送tcp
					if(g_u8TcpOver == TCP_OVER)
					{
						tcp_send_sersonTd();
					}
					else
					{
						//lt:关闭tcp传输
						tcp_close();
					}
					tos_task_delay(50);//lt:延时50ms去申请mqtt发送
//					if(g_u8MqttOver == MQTT_OVER)
//					{
//						mqtt_sensor_pub();
//					}
					//lt:释放传感器buffer
				  memset(g_sensorTd.pxvibValue, 0, sizeof(float) * MAX_XYZSAMPLE_CNT);
	        memset(g_sensorTd.pyvibValue, 0, sizeof(float) * MAX_XYZSAMPLE_CNT);
	        memset(g_sensorTd.pzvibValue, 0, sizeof(float) * MAX_XYZSAMPLE_CNT);
					tos_mmheap_free(g_sensorTd.pxvibValue);
					g_sensorTd.pxvibValue = NULL;
					tos_mmheap_free(g_sensorTd.pyvibValue);
					g_sensorTd.pyvibValue = NULL;
					tos_mmheap_free(g_sensorTd.pzvibValue);
					g_sensorTd.pzvibValue = NULL;
					//lt:重新设置RTC时间
					if(again_set_sample_time() != 0)
					{
						return;
					}
					//lt:复位测试标志
					g_ttcpfin_flag = 0;
					g_ntcpfin_flag = 0;
					g_vtcpfin_flag = 0;
					continue;
				}
				//lt:发布正常数据
				//printf("publish normal data\n");
				if(get_unix_time(&g_unixTime.mqtt_unix_et) != 0)
		    {
			    return;
		    }//lt:采集完成获得unix时间
				if(g_u8MqttOver == MQTT_OVER)
				{
					mqtt_sensor_pub();
				}
				//lt:延时10ms去再次申请mqtt采集发送
				tos_task_delay(10);
				if(get_unix_time(&g_unixTime.mqtt_unix_st) != 0)
		    {
			    return;
		    }//lt:采集开始获得unix时间
				tos_sem_post(&sem_tsample_mqtt);
				tos_sem_post(&sem_nsample_mqtt);
				tos_sem_post(&sem_vsample_mqtt);
			}
			
			if(flag_match == ev_exce_pub)
			{
				//lt:发布异常数据
				//lt:检查mqtt的连接的状态
				//printf("publish exce data\n");
				if(g_u8MqttOver == MQTT_OVER)
				{
					mqtt_alarms_pub();
				}
			}
			
			if(flag_match == ev_para_pub)
			{
				//lt:发布参数数据
			}
		}
	}
	return;
}

/*********MQTTPUB_TASK END***********/