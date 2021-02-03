/**
  ******************************************************************************
  * @file           : mqttsub.c
  * @brief          : mqttsub program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "mqttsub.h"
#include "esp32.h"
#include "fs.h"
#include "include.h"
#include "bsp.h"
#include "task.h"

#define MQTT_CONN_URL "10.1.207.239"
//#define MQTT_CONN_URL "mqtt.iot.360humi.com"
#define MQTT_CONN_PORT 1883
#define MAX_MQTTUSERCFG_STR 256
#define MAX_MQTTCONN_STR 64
#define MAX_MQTTSUB_STR 128

#define MAX_TOPIC_STR 16
#define MAX_SUBVALUE_STR 64
#define SAMPLE_TOPIC_STR "/samplePara"
#define DEV_TOPIC_STR "/devicePara"
#define COLL_TOPIC_STR "/collControl"

mqttRevData g_mqttRevData;
samplePara g_samplePara;
collControl g_collControl;

void mqttsub_init()
{
	g_mqttRevData.pmq_rev_data = NULL;
	g_mqttRevData.mq_rev_len = 0;
	
	memset(g_samplePara.h5gecId, 0, MAX_SUB_H5GID_STR + 1);
	g_samplePara.time = 0;
	g_samplePara.lastTime = 0;
	g_samplePara.interTime = 0;
	g_samplePara.voiceFrequenc = 0;
	g_samplePara.vibFrequenc = 0;
	
	memset(g_collControl.h5gecId, 0, MAX_SUB_H5GID_STR + 1);
	g_collControl.time = 0;
	g_collControl.continueColl = 0;
	g_collControl.awake = 0;
}

static int esp32_mqttusercfg_set(char* mqttusercfg_str)
{
	at_echo_t echo;
	
	tos_at_echo_create(&echo, NULL, 0, NULL);
	tos_at_cmd_exec(&echo, 1000, mqttusercfg_str);
	if (echo.status == AT_ECHO_STATUS_OK) {
      return 0;
  }
  return -1;
}

static int esp32_mqttconn(char* mqttconn_str)
{
	int try = 0;
	at_echo_t echo;
	
	tos_at_echo_create(&echo, NULL, 0, "OK");
	while (try++ < 10) {
        tos_at_cmd_exec_until(&echo, 15000, mqttconn_str);
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int esp32_mqttsub(char* mqttsub_str)
{
	int try = 0;
	at_echo_t echo;
	
	tos_at_echo_create(&echo, NULL, 0, "OK");
	while (try++ < 10) {
        tos_at_cmd_exec_until(&echo, 15000, mqttsub_str);
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int esp32_mqtt_clean()
{
	at_echo_t echo;
	
	tos_at_echo_create(&echo, NULL, 0, "OK");
	tos_at_cmd_exec_until(&echo, 15000, "AT+MQTTCLEAN=0\r\n");
	
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
	return -1;
		
}

int mqtt_sub_fun()
{
	uint8_t* pdevgid;
	uint8_t* pdevuuid;
	
	char mqttsub_str[MAX_MQTTSUB_STR + 1];
	
	pdevgid = g_fsData.devgid;
	pdevuuid = g_fsData.devuuid;
	
	//lt:开始订阅消息
	//lt:构造MQTTSUB参数字符串
	snprintf(mqttsub_str, MAX_MQTTSUB_STR, "AT+MQTTSUB=0,\"%s/%s/samplePara\",0\r\n", pdevgid, pdevuuid);
	if(esp32_mqttsub(mqttsub_str) != 0)
	{
		printf("sub mqtt failed");
		return -1;
	}
	
	tos_task_delay(500);
	memset(mqttsub_str, 0, MAX_MQTTSUB_STR + 1);
	snprintf(mqttsub_str, MAX_MQTTSUB_STR, "AT+MQTTSUB=0,\"%s/%s/collControl\",0\r\n", pdevgid, pdevuuid);
	
	if(esp32_mqttsub(mqttsub_str) != 0)
	{
		printf("sub mqtt failed");
		return -1;
	}
	
	return 0;
}

int mqtt_conn_fun()
{
	
	uint8_t* pmqclientid;
	uint8_t* pmqusername;
	uint8_t* pmqpassword;
	
	char mqttusercfg_str[MAX_MQTTUSERCFG_STR];
	char mqttconn_str[MAX_MQTTCONN_STR];
	
	//lt:读取fs参数
	pmqclientid = g_fsData.mqclientid;
	pmqusername = g_fsData.mqusername;
	pmqpassword = g_fsData.mqpassword;
	
	//lt:构造MQTTUSERCFG参数字符串
	snprintf(mqttusercfg_str, MAX_MQTTUSERCFG_STR, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", pmqclientid, pmqusername, pmqpassword);
	//lt:构造MQTTCONN参数字符串
	snprintf(mqttconn_str, MAX_MQTTCONN_STR, "AT+MQTTCONN=0,\"%s\",%d,0\r\n", MQTT_CONN_URL, MQTT_CONN_PORT);
	
	if(esp32_mqttusercfg_set(mqttusercfg_str) != 0)
	{
		return -1;
	}
	
	if(esp32_mqttconn(mqttconn_str) != 0)
	{
		printf("conn mqtt failed");
		return -1;
	}
	
	tos_task_delay(1000);
	
	return 0;
	
}

int mqtt_clean()
{
	if(esp32_mqtt_clean() != 0)
	{
		return -1;
	}
	return 0;
}

static void json_get_para(char* para_value, char* para_str, char* ref_str)
{
	int i = strlen(ref_str);
  int j = 0;
	while(1)
	{
		if(para_str[i] == ',')
		{
			break;
		}
		para_value[j++] = para_str[i++];
	}
	return;
}

static void json_get_last_para(char* para_value, char* para_str, char* ref_str)
{
	int i = strlen(ref_str);
  int j = 0;
	while(1)
	{
		if(para_str[i] == '}')
		{
			break;
		}
		para_value[j++] = para_str[i++];
	}
	return;
}

int mqtt_rev_json(char* pmessage_data)
{
	char* ptopic = NULL;
	char* pkey = NULL;
	
	char s_value[MAX_SUBVALUE_STR] = {0};
	int u32_value;
	
	char sub_value[MAX_SUBVALUE_STR] = {0};
	ptopic = strstr(pmessage_data, SAMPLE_TOPIC_STR);
	if(ptopic == NULL)
	{
		ptopic = strstr(pmessage_data, DEV_TOPIC_STR);
		if(ptopic == NULL)
		{
			ptopic = strstr(pmessage_data, COLL_TOPIC_STR);
			if(ptopic == NULL)
			{
				return -1;
			}
			else
			{
				//COLL_TOPIC_STR
				pkey = strstr(ptopic, "\"continueColl\":");
		    if(pkey == NULL)
		    {
			    return -1;
		    }
		    else
		    {
			    json_get_para(sub_value, pkey, "\"continueColl\":");
			    printf("sub_value is %s\n", sub_value);
			    sscanf(sub_value, "\"%d\"", &u32_value); //lt:赋值
			    g_collControl.continueColl = (uint32_t)u32_value;
					//lt:唤醒,采样去判断该持续采集还是按照配置参数采集
					if(g_collControl.continueColl == 1 && g_u8MqttOver == MQTT_OVER)
					{
						tos_event_post(&ev_tsample, ev_wake_tsample);
	          tos_event_post(&ev_nsample, ev_wake_nsample);
	          tos_event_post(&ev_vsample, ev_wake_vsample);
						
						tos_sem_post(&sem_tsample_mqtt);
		        tos_sem_post(&sem_nsample_mqtt);
		        tos_sem_post(&sem_vsample_mqtt);
					}
			    u32_value = 0;
			    memset(sub_value, 0, MAX_SUBVALUE_STR);
		    }
				
				pkey = strstr(ptopic, "\"awake\":");
		    if(pkey == NULL)
		    {
			    return -1;
		    }
		    else
		    {
			    json_get_last_para(sub_value, pkey, "\"awake\":");
			    printf("sub_value is %s\n", sub_value);
			    sscanf(sub_value, "\"%d\"", &u32_value); //lt:赋值
			    g_collControl.awake = (uint32_t)u32_value;
					//lt:持续采集和唤醒进行互斥
					if(g_collControl.continueColl == 1)
					{
						g_collControl.awake = 0;
					}
					//lt:唤醒
					if(g_collControl.awake == 1 && g_u8MqttOver == MQTT_OVER)
					{
						tos_event_post(&ev_tsample, ev_wake_tsample);
	          tos_event_post(&ev_nsample, ev_wake_nsample);
	          tos_event_post(&ev_vsample, ev_wake_vsample);
						
						tos_sem_post(&sem_tsample_mqtt);
		        tos_sem_post(&sem_nsample_mqtt);
		        tos_sem_post(&sem_vsample_mqtt);
					}
			    u32_value = 0;
			    memset(sub_value, 0, MAX_SUBVALUE_STR);
		    }
			}
		}
		else
		{
			//DEV_TOPIC_STR
		}
	}
	else
	{
		//SAMPLE_TOPIC_STR
		pkey = strstr(ptopic, "\"lastTime\":");
		if(pkey == NULL)
		{
			return -1;
		}
		else
		{
			json_get_para(sub_value, pkey, "\"lastTime\":");
			printf("sub_value is %s\n", sub_value);
			sscanf(sub_value, "\"%d\"", &u32_value); //lt:赋值
			g_samplePara.lastTime = (uint32_t)u32_value;
			u32_value = 0;
			memset(sub_value, 0, MAX_SUBVALUE_STR);
		}
		
		pkey = strstr(ptopic, "\"interTime\":");
		if(pkey == NULL)
		{
			return -1;
		}
		else
		{
			json_get_para(sub_value, pkey, "\"interTime\":");
			printf("sub_value is %s\n", sub_value);
			sscanf(sub_value, "\"%d\"", &u32_value); //lt:赋值
			g_samplePara.interTime = (uint32_t)u32_value;
			u32_value = 0;
			memset(sub_value, 0, MAX_SUBVALUE_STR);
		}
		
		pkey = strstr(ptopic, "\"vibFrequenc\":");
		if(pkey == NULL)
		{
			return -1;
		}
		else
		{
			json_get_para(sub_value, pkey, "\"vibFrequenc\":");
			printf("sub_value is %s\n", sub_value);
			sscanf(sub_value, "\"%d\"", &u32_value); //lt:赋值
			g_samplePara.vibFrequenc = (uint32_t)u32_value;
			u32_value = 0;
			memset(sub_value, 0, MAX_SUBVALUE_STR);
		}
		
		pkey = strstr(ptopic, "\"voiceFrequenc\":");
		if(pkey == NULL)
		{
			return -1;
		}
		else
		{
			json_get_last_para(sub_value, pkey, "\"voiceFrequenc\":");
			printf("sub_value is %s\n", sub_value);
			sscanf(sub_value, "\"%d\"", &u32_value); //lt:赋值
			g_samplePara.vibFrequenc = (uint32_t)u32_value;
			u32_value = 0;
			memset(sub_value, 0, MAX_SUBVALUE_STR);
		}
	}
	
	return 0;
	
}

