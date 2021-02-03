/**
  ******************************************************************************
  * @file           : mqttsub.h
  * @brief          : Header for mqttsub.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
  
#ifndef _MQTTSUB_H
#define _MQTTSUB_H

#include "sal_module_wrapper.h"
#include "tos_at.h"
#include "cmsis_os.h"

#define MAX_SUB_H5GID_STR 32

extern int mqtt_conn_fun();
extern int mqtt_sub_fun();
extern void mqttsub_init();
extern int mqtt_clean();

extern int mqtt_rev_json(char* pmessage_data);

typedef struct _mqttRevData
{
	uint8_t* pmq_rev_data;
	uint32_t mq_rev_len;
}mqttRevData;

typedef struct _samplePara
{
	uint8_t h5gecId[MAX_SUB_H5GID_STR + 1];
	uint32_t time;
	uint32_t lastTime;
	uint32_t interTime;
	uint32_t vibFrequenc;
	uint32_t voiceFrequenc;
}samplePara;

typedef struct _collControl
{
	uint8_t h5gecId[MAX_SUB_H5GID_STR + 1];
	uint32_t time;
	uint8_t continueColl;
	uint8_t awake;
}collControl;

extern mqttRevData g_mqttRevData;
extern samplePara g_samplePara;
extern collControl g_collControl;

#endif