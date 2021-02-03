/**
  ******************************************************************************
  * @file           : tcpsend.c
  * @brief          : tcpsend program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "tcpsend.h"
#include "esp32.h"
#include "include.h"
#include "bsp.h"
#include "fs.h"
#include "mqttpub.h"

#define MAX_TCP_SEND_MSG 2048

#if (MAX_XYZSAMPLE_CNT == 20)
#define XYZDATA_PRINTF_D "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f"
#define _TOSTR(s) #s
#define XDATA_PRINTF_VAL pxvibValue[0], pxvibValue[1], pxvibValue[2], pxvibValue[3], pxvibValue[4], pxvibValue[5], pxvibValue[6], pxvibValue[7], pxvibValue[8], pxvibValue[9], pxvibValue[10], pxvibValue[11], pxvibValue[12], pxvibValue[13], pxvibValue[14], pxvibValue[15], pxvibValue[16], pxvibValue[17], pxvibValue[18], pxvibValue[19]
#define YDATA_PRINTF_VAL pyvibValue[0], pyvibValue[1], pyvibValue[2], pyvibValue[3], pyvibValue[4], pyvibValue[5], pyvibValue[6], pyvibValue[7], pyvibValue[8], pyvibValue[9], pyvibValue[10], pyvibValue[11], pyvibValue[12], pyvibValue[13], pyvibValue[14], pyvibValue[15], pyvibValue[16], pyvibValue[17], pyvibValue[18], pyvibValue[19]
#define ZDATA_PRINTF_VAL pzvibValue[0], pzvibValue[1], pzvibValue[2], pzvibValue[3], pzvibValue[4], pzvibValue[5], pzvibValue[6], pzvibValue[7], pzvibValue[8], pzvibValue[9], pzvibValue[10], pzvibValue[11], pzvibValue[12], pzvibValue[13], pzvibValue[14], pzvibValue[15], pzvibValue[16], pzvibValue[17], pzvibValue[18], pzvibValue[19]
//#define SENSOR_PUB_PRINTF(X, Y, Z) "AT+MQTTPUB=0,\"%s/%s/sensorTd\",\"{\\\"sensorTd\\\":{\\\"h5gecId\\\":\\\"%s\\\"\\,\\\"startTime\\\":\\\"%d\\\"\\,\\\"endTime\\\"\\,\\\"%d\\\"\\,\\\"tempValue\\\"\\,\\\"%.1f\\\"\\,\\\"voiceValue\\\"\\,\\\"%.1f\\\"\\,\\\"xvibValue\\\"\\,\\\""X"\\\",\\\"yvibValue\\\":\\\""Y"\\\",\\\"zvibValue\\\":\\\""Z"\\\"}}\",0,0\r\n"
#define SENSOR_TCP_PRINTF(X, Y, Z) "{\"flag\":\"start\",\"h5gecId\":\"%s\",\"startTime\":\"%ld000\",\"lastTime\",\"%d\",\"tempValue\",\"%.1f\",\"voiceValue\",\"%.1f\",\"xvibValue\",\""X"\",\"yvibValue\",\""Y"\",\"zvibValue\",\""Z"\"}"
#endif

tcpSend g_tcpSend;

static int tcp_send(char* tcp_send_str, uint16_t msg_len)
{
	int socket_id;
	socket_id = g_tcpSend.socketId;
	tos_sal_module_send(socket_id, (uint8_t*)tcp_send_str, msg_len);
}

void tcp_init()
{
	memset(g_tcpSend.h5gecId, 0, MAX_TCP_H5GID_STR + 1);
	
	g_tcpSend.socketId = -1;
	
	g_tcpSend.startTime = 0;
	g_tcpSend.lastTime = 0;
	
	g_tcpSend.tempValue = 0;
	g_tcpSend.pvoiceValue = NULL;
	g_tcpSend.pxvibValue = NULL;
	g_tcpSend.pyvibValue = NULL;
	g_tcpSend.pzvibValue = NULL;
}

int tcp_conn(char* ip, char* port)
{
	int socket_id = -1;
	
	socket_id = tos_sal_module_connect(ip, port, TOS_SAL_PROTO_TCP);
	
	g_tcpSend.socketId = socket_id;
}

int tcp_send_sersonTd()
{
	char* ptcp_send_msg = NULL;
	ptcp_send_msg = (char*)tos_mmheap_alloc(MAX_TCP_SEND_MSG);
	
	uint16_t msg_len = 0;
	
	memcpy(g_tcpSend.h5gecId, "1354750127563923457", MAX_TCP_H5GID_STR);
	long startTime = g_unixTime.tcp_unix_st;
	printf("startTime is %ld\n", startTime);
	int lastTime = 4;//lttest
	
	float* pxvibValue = g_sensorTd.pxvibValue;
	float* pyvibValue = g_sensorTd.pyvibValue;
	float* pzvibValue = g_sensorTd.pzvibValue;
	
	snprintf(ptcp_send_msg, MAX_TCP_SEND_MSG, 
	SENSOR_TCP_PRINTF(XYZDATA_PRINTF_D, XYZDATA_PRINTF_D, XYZDATA_PRINTF_D), 
	g_tcpSend.h5gecId, startTime, lastTime, g_sensorTd.tempValue, 12.2, XDATA_PRINTF_VAL, YDATA_PRINTF_VAL, ZDATA_PRINTF_VAL);
	
	tcp_send(ptcp_send_msg, strlen(ptcp_send_msg));
	
	memset(ptcp_send_msg, 0, MAX_TCP_SEND_MSG);
	tos_mmheap_free(ptcp_send_msg);
	ptcp_send_msg = NULL;
}

int tcp_close()
{
	int socket_id;
	socket_id = g_tcpSend.socketId;
	tos_sal_module_close(socket_id);
}
