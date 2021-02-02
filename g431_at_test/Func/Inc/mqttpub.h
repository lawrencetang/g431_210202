/**
  ******************************************************************************
  * @file           : mqttpub.h
  * @brief          : Header for mqttpub.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _MQTTPUB_H
#define _MQTTPUB_H

#include "sal_module_wrapper.h"
#include "tos_at.h"
#include "cmsis_os.h"

#define MAX_PUB_H5GID_STR 32
#define MAX_PUB_VOICE_AMOUNT 100
#define MAX_PUB_X_AMOUNT 100
#define MAX_PUB_Y_AMOUNT 100
#define MAX_PUB_Z_AMOUNT 100

#define MAX_XYZSAMPLE_CNT 20

extern int mqtt_sensor_pub();
extern int mqtt_alarms_pub();
extern void mqttpub_init();

typedef struct _sensorTd
{
	uint8_t h5gecId[MAX_PUB_H5GID_STR + 1];
	uint32_t time;
	float tempValue;
	float* pvoiceValue;
	float* pxvibValue;
	float* pyvibValue;
	float* pzvibValue;
}sensorTd;

typedef struct _alarms
{
	uint8_t h5gecId[MAX_PUB_H5GID_STR + 1];
	uint32_t time;
	uint8_t tempFault;
	uint8_t voiceFault;
	uint8_t vibFault;
	uint8_t otherhwFault;
	uint8_t lowPower;
}alarms;

extern sensorTd g_sensorTd;
extern alarms g_alarms;

#endif
