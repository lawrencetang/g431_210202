/**
  ******************************************************************************
  * @file           : mqttpub.c
  * @brief          : mqttpub program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "mqttpub.h"
#include "esp32.h"
#include "include.h"
#include "bsp.h"
#include "fs.h"

#define MAX_MQTT_PUB_ALARMS_MSG 2048
#define MAX_MQTT_PUB_SENSORTD_MSG 2048

#if (MAX_XYZSAMPLE_CNT == 20)
#define XYZDATA_PRINTF_D "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
#define _TOSTR(s) #s
#define XDATA_PRINTF_VAL pxvibValue[0], pxvibValue[1], pxvibValue[2], pxvibValue[3], pxvibValue[4], pxvibValue[5], pxvibValue[6], pxvibValue[7], pxvibValue[8], pxvibValue[9], pxvibValue[10], pxvibValue[11], pxvibValue[12], pxvibValue[13], pxvibValue[14], pxvibValue[15], pxvibValue[16], pxvibValue[17], pxvibValue[18], pxvibValue[19]
#define YDATA_PRINTF_VAL pyvibValue[0], pyvibValue[1], pyvibValue[2], pyvibValue[3], pyvibValue[4], pyvibValue[5], pyvibValue[6], pyvibValue[7], pyvibValue[8], pyvibValue[9], pyvibValue[10], pyvibValue[11], pyvibValue[12], pyvibValue[13], pyvibValue[14], pyvibValue[15], pyvibValue[16], pyvibValue[17], pyvibValue[18], pyvibValue[19]
#define ZDATA_PRINTF_VAL pzvibValue[0], pzvibValue[1], pzvibValue[2], pzvibValue[3], pzvibValue[4], pzvibValue[5], pzvibValue[6], pzvibValue[7], pzvibValue[8], pzvibValue[9], pzvibValue[10], pzvibValue[11], pzvibValue[12], pzvibValue[13], pzvibValue[14], pzvibValue[15], pzvibValue[16], pzvibValue[17], pzvibValue[18], pzvibValue[19]
//#define SENSOR_PUB_PRINTF(X, Y, Z) "AT+MQTTPUB=0,\"%s/%s/sensorTd\",\"{\\\"sensorTd\\\":{\\\"h5gecId\\\":\\\"%s\\\"\\,\\\"startTime\\\":\\\"%d\\\"\\,\\\"endTime\\\"\\,\\\"%d\\\"\\,\\\"tempValue\\\"\\,\\\"%.1f\\\"\\,\\\"voiceValue\\\"\\,\\\"%.1f\\\"\\,\\\"xvibValue\\\"\\,\\\""X"\\\",\\\"yvibValue\\\":\\\""Y"\\\",\\\"zvibValue\\\":\\\""Z"\\\"}}\",0,0\r\n"
#define SENSOR_PUB_PRINTF(X, Y, Z) "{\"sensorTd\":{\"h5gecId\":\"%s\",\"startTime\":\"%ld000\",\"endTime\":\"%ld000\",\"tempValue\":\"%.1f\",\"voiceValue\":\"%.1f\",\"xvibValue\":\""X"\",\"yvibValue\":\""Y"\",\"zvibValue\":\""Z"\"}}"
#endif

sensorTd g_sensorTd;
alarms g_alarms;

void mqttpub_init()
{
	memset(g_sensorTd.h5gecId, 0, MAX_PUB_H5GID_STR + 1);
	memset(g_alarms.h5gecId, 0, MAX_PUB_H5GID_STR + 1);
	
	g_sensorTd.time = 0;
	g_alarms.time = 0;
	
	g_sensorTd.tempValue = 0;
	g_sensorTd.pvoiceValue = NULL;
	g_sensorTd.pxvibValue = NULL;
	g_sensorTd.pyvibValue = NULL;
	g_sensorTd.pzvibValue = NULL;
	
	g_alarms.lowPower = 0;
	g_alarms.otherhwFault = 0;
	g_alarms.tempFault = 0;
	g_alarms.voiceFault = 0;
	g_alarms.vibFault = 0;
	
}

static int esp32_mqttpubraw(char* mqttpubraw_str)
{
	int try = 0;
	at_echo_t echo;
	char echo_buffer[64];
	
	tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), ">");
	while (try++ < 10) {
		    tos_at_cmd_exec_until(&echo, 15000, mqttpubraw_str);
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

static int esp32_sendraw(uint8_t* pmsg, uint16_t msg_len)
{
	at_echo_t echo;
	tos_at_echo_create(&echo, NULL, 0, "+MQTTPUB:OK");
	
	tos_at_raw_data_send_until(&echo, 15000, pmsg, msg_len);
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
	return -1;
}

static int esp32_mqttpub(char* mqttpub_str)
{
	int try = 0;
	at_echo_t echo;
	
	tos_at_echo_create(&echo, NULL, 0, "OK");
	while (try++ < 10) {
        tos_at_cmd_exec_until(&echo, 15000, mqttpub_str);
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

int mqtt_sensor_pub()
{
	char* pmqtt_pub_sensortd_msg = NULL;
	pmqtt_pub_sensortd_msg = (char*)tos_mmheap_alloc(MAX_MQTT_PUB_SENSORTD_MSG);
	char mqtt_pub_topic_str[128] = {0};
	uint16_t msg_len = 0;
	
	uint8_t* pdevgid;
	uint8_t* pdevuuid;
	
	long startTime = g_unixTime.mqtt_unix_st;
	long endTime = g_unixTime.mqtt_unix_et;
	
	float* pxvibValue = g_sensorTd.pxvibValue;
	float* pyvibValue = g_sensorTd.pyvibValue;
	float* pzvibValue = g_sensorTd.pzvibValue;
	
	pdevgid = g_fsData.devgid;
	pdevuuid = g_fsData.devuuid;
	
	memcpy(g_sensorTd.h5gecId, "1354750127563923457", MAX_PUB_H5GID_STR);
	
//	printf("temp data is %.1f\n", g_sensorTd.tempValue);
//	for(int i = 0; i < MAX_XYZSAMPLE_CNT; i++)
//	{
//		printf("vx%d is %.2f\n", i, g_sensorTd.pxvibValue[i]);
//		printf("vy%d is %.2f\n", i, g_sensorTd.pyvibValue[i]);
//		printf("zx%d is %.2f\n", i, g_sensorTd.pzvibValue[i]);
//	}
	
	//lt:开始组json格式进行mqtt发送
  snprintf(pmqtt_pub_sensortd_msg, MAX_MQTT_PUB_SENSORTD_MSG, 
	SENSOR_PUB_PRINTF(XYZDATA_PRINTF_D, XYZDATA_PRINTF_D, XYZDATA_PRINTF_D), 
	g_sensorTd.h5gecId, startTime, endTime, g_sensorTd.tempValue, 12.2, XDATA_PRINTF_VAL, YDATA_PRINTF_VAL, ZDATA_PRINTF_VAL);
	//printf("%s\n", pmqtt_pub_sensortd_msg);
	
  snprintf(mqtt_pub_topic_str, 128, "AT+MQTTPUBRAW=0,\"%s/%s/1354375976365907969\",%d,0,0\r\n", pdevgid, pdevuuid, strlen(pmqtt_pub_sensortd_msg));
	//printf("%s\n", mqtt_pub_topic_str);
	
	if(esp32_mqttpubraw(mqtt_pub_topic_str) != 0)
	{
		printf("pub topic mqtt failed");
		return -1;
	}
	
	if(esp32_sendraw((uint8_t*)pmqtt_pub_sensortd_msg, strlen(pmqtt_pub_sensortd_msg)) != 0)
	{
		printf("pub serson mqtt failed");
		return -1;
	}
	
	memset(pmqtt_pub_sensortd_msg, 0, MAX_MQTT_PUB_SENSORTD_MSG);
	tos_mmheap_free(pmqtt_pub_sensortd_msg);
	pmqtt_pub_sensortd_msg = NULL;
	
	//lt:处理完清空分配的空间,但不释放,等待tcpfinish后再tcp的task去释放
	memset(g_sensorTd.pxvibValue, 0, sizeof(float) * MAX_XYZSAMPLE_CNT);
	memset(g_sensorTd.pyvibValue, 0, sizeof(float) * MAX_XYZSAMPLE_CNT);
	memset(g_sensorTd.pzvibValue, 0, sizeof(float) * MAX_XYZSAMPLE_CNT);
//	tos_mmheap_free(g_sensorTd.pxvibValue);
//	g_sensorTd.pxvibValue = NULL;
//	tos_mmheap_free(g_sensorTd.pyvibValue);
//	g_sensorTd.pyvibValue = NULL;
//	tos_mmheap_free(g_sensorTd.pzvibValue);
//	g_sensorTd.pzvibValue = NULL;
}

int mqtt_alarms_pub()
{
	//char mqtt_pub_alarms_msg[MAX_MQTT_PUB_ALARMS_MSG + 1];
	char* pmqtt_pub_alarms_msg = NULL;
	pmqtt_pub_alarms_msg = (char*)tos_mmheap_alloc(MAX_MQTT_PUB_ALARMS_MSG);
	
	uint8_t* pdevgid;
	uint8_t* pdevuuid;
	
	pdevgid = g_fsData.devgid;
	pdevuuid = g_fsData.devuuid;
	
	memcpy(g_alarms.h5gecId, "1354750127563923457", MAX_PUB_H5GID_STR);
	
	
	snprintf(pmqtt_pub_alarms_msg, MAX_MQTT_PUB_ALARMS_MSG, 
	"AT+MQTTPUB=0,\"%s/%s/alarms\",\"{\\\"alarms\\\":{\\\"h5gecId\\\":\\\"%s\\\"\\,\\\"time\\\":\\\"%d\\\"\\,\\\"tempFault\\\"\\,\\\"%d\\\"\\,\\\"voiceFault\\\"\\,\\\"%d\\\"\\,\\\"vibFault\\\"\\,\\\"%d\\\"\\,\\\"otherhwFault\\\"\\,\\\"%d\\\"\\,\\\"lowPower\\\"\\,\\\"%d\\\"}}\",0,0\r\n", 
	pdevgid, pdevuuid, g_alarms.h5gecId, g_alarms.time, g_alarms.tempFault, g_alarms.voiceFault, g_alarms.vibFault, g_alarms.otherhwFault, g_alarms.lowPower);
	
	if(esp32_mqttpub(pmqtt_pub_alarms_msg) != 0)
	{
		printf("pub alarms mqtt failed");
		return -1;
	}
	
	memset(pmqtt_pub_alarms_msg, 0, 4096);
	tos_mmheap_free(pmqtt_pub_alarms_msg);
	pmqtt_pub_alarms_msg = NULL;
}

