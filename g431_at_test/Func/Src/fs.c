/**
  ******************************************************************************
  * @file           : fs.c
  * @brief          : fs program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "fs.h"
#include "esp32.h"

fsData g_fsData;

void fs_init()
{
	memset(g_fsData.wifissid, 0, MAX_WIFISSID_STRING);
	memset(g_fsData.wifipassword, 0, MAX_WIFIPASSWORD_STRING);
	memset(g_fsData.devgid, 0, MAX_DEVGID_STRING);
	memset(g_fsData.devuuid, 0, MAX_DEVUUID_STRING);
	memset(g_fsData.mqclientid, 0, MAX_MQCLIENTID_STRING);
	memset(g_fsData.mqusername, 0, MAX_MQUSERNAME_STRING);
	memset(g_fsData.mqpassword, 0, MAX_MQPASSWORD_STRING);
}

int fs_read_file(char* filename)
{
	
}
