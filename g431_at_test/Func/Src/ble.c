/**
  ******************************************************************************
  * @file           : ble.c
  * @brief          : ble program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "ble.h"
#include "esp32.h"
#include "fs.h"

static int esp32ble_at(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "AT\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
		printf("echo.status is %d\n", echo.status);
    return -1;
}

static int esp32ble_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

int esp32ble_init_at(void)
{
	printf("Init ESP32ble ...\n" );
	
	if (esp32ble_at() != 0) {
        printf("esp32ble echo close FAILED\n");
        return -1;
    }
	//printf("Init ESP32ble complete\n" );
	
	return 0;
}

int ble_resolve_para(char* conf_data)
{
	char* token;
	if(conf_data[strlen(conf_data) - 2] == '\r')
	{
		token = strtok(conf_data, "///");
		if(token == NULL)
		{
			return -1;
		}
		memcpy(g_fsData.wifissid, token, strlen(token));
		printf("wifissid is %s\n", g_fsData.wifissid);
		
		token = strtok(NULL, "///");
		if(token == NULL)
		{
			return -1;
		}
		memcpy(g_fsData.wifipassword, token, strlen(token));
		printf("wifissid is %s\n", g_fsData.wifipassword);
		
		token = strtok(NULL, "///");
		if(token == NULL)
		{
			return -1;
		}
		memcpy(g_fsData.devgid, token, strlen(token));
		
		token = strtok(NULL, "///");
		if(token == NULL)
		{
			return -1;
		}
		memcpy(g_fsData.devuuid, token, strlen(token));
		
		token = strtok(NULL, "///");
		if(token == NULL)
		{
			return -1;
		}
		memcpy(g_fsData.mqclientid, token, strlen(token));
		
		token = strtok(NULL, "///");
		if(token == NULL)
		{
			return -1;
		}
		if(strcmp(token, "-") != 0)
		{
			memcpy(g_fsData.mqusername, token, strlen(token));
		}
		
		token = strtok(NULL, "///");
		
		if(token == NULL)
		{
			return -1;
		}
		if(strcmp(token, "-") != 0)
		{
			memcpy(g_fsData.mqpassword, token, strlen(token));
		}
		
		token = strtok(NULL, "///");
		printf("last token is %s", token);
		if(strcmp(token, "stop\r\n") != 0)
		{
			return -1;
		}
		
		//lt:½âÎö½áÊø
		token = strtok(NULL, "///");
		if(token != NULL)
		{
			return -1;
		}
		
	}
	else
	{
		return -1;
	}
	return 0;
}