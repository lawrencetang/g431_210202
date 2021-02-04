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

static int esp32_bleinit()
{
	at_echo_t echo;
	
	tos_at_echo_create(&echo, NULL, 0, "OK");
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEINIT=2\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_blegattssrvcre()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEGATTSSRVCRE\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_blegattssrvstart()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEGATTSSRVSTART\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_set_blename()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLENAME=\"Espressif\"\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_set_bleaddr()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEADDR=1,\"F8:7F:24:87:1C:F7\"\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_set_bleadvparam()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEADVPARAM=50,50,0,0,7\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_set_bleadvdata()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEADVDATA=\"0201061AFF4C000215FDA50693A4E24FB1AFCFC6EB0764782527B7F206C5\"\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
		return 0;
    }
	return -1;
}

static int esp32_set_bleadvstart()
{
	at_echo_t echo;
	tos_at_cmd_exec_until(&echo, 15000, "AT+BLEADVSTART\r\n");
	if (echo.status == AT_ECHO_STATUS_EXPECT) {
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

int ble_rec_data(char* rec_str)
{
	if(esp32_bleinit() != 0)
	{
		return -1;
	}
	
	if(esp32_set_blename() != 0)
	{
		return -1;
	}
	
	if(esp32_set_bleaddr() != 0)
	{
		return -1;
	}
	
	if(esp32_set_bleadvparam() != 0)
	{
		return -1;
	}
	
	if(esp32_blegattssrvcre() != 0)
	{
		return -1;
	}
	
	if(esp32_blegattssrvstart() != 0)
	{
		return -1;
	}
	
	if(esp32_set_bleadvstart() != 0)
	{
		return -1;
	}
	return 0;
}