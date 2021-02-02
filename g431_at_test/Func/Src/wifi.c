/**
  ******************************************************************************
  * @file           : wifi.c
  * @brief          : wifi program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "wifi.h"
#include "esp32.h"
#include "fs.h"

#include "cmsis_os.h"

uint8_t wifi_recv_data[WIFI_RECV_LEN];

int esp32wifi_init_at()
{
	printf("Init ESP32 wifi ...\n" );

    if (esp32_restore() != 0) {
        printf("esp32 restore FAILED\n");
        return -1;
    }
    
    if (esp32_echo_close() != 0) {
        printf("esp32 echo close FAILED\n");
        return -1;
    }

    if (esp32_net_mode_set(ESP32_NET_MODE_STA) != 0) {
        printf("esp32 net mode set FAILED\n");
        return -1;
    }

    if (esp32_send_mode_set(ESP32_SEND_MODE_NORMAL) != 0) {
        printf("esp32 send mode set FAILED\n");
        return -1;
    }

#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    if (esp32_multilink_set(ESP32_MULTILINK_STATE_DISABLE) != 0) {
        printf("esp32 multilink set FAILED\n");
        return -1;
    }
#else
    if (esp32_multilink_set(ESP32_MULTILINK_STATE_ENABLE) != 0) {
        printf("esp32 multilink set FAILED\n");
        return -1;
    }
#endif

    printf("Init ESP32 wifi Done\n" );
    return 0;
}

int esp32_join_ap(const char *ssid, const char *pwd)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "OK");
    while (try++ < 10) {
        tos_at_cmd_exec_until(&echo, 15000, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
        if (echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

/*
**test wifi fun
*/
void esp32_test_wifi()
{
  int socket_id = -1;
  int recv_len = -1;

  /* 通信模组初始化 */
  //esp8266_sal_init(HAL_UART_PORT_1); //ltg
	esp32wifi_init_at();

  /* 入网 */
  esp32_join_ap("HMW-BG","humiwang666");

  /* 发起socket连接 */
  socket_id = tos_sal_module_connect("10.1.205.57", "6666", TOS_SAL_PROTO_TCP);

  /* 发送一次数据 */
  tos_sal_module_send(socket_id, (uint8_t*)"hello,server!", 12);
   /* 接收一次数据 */
  recv_len = tos_sal_module_recv_timeout(socket_id, wifi_recv_data, sizeof(wifi_recv_data), 8000);

  if (recv_len < 0) {
       printf("receive error\n");
   } else if (recv_len == 0) {
       printf("receive none\n");
   } else {
       wifi_recv_data[recv_len] = 0;
       printf("receive len: %d\nmsg from remote: %s\n", recv_len, wifi_recv_data);
   }

  /* 关闭socket */
  tos_sal_module_close(socket_id);
	 return;
}

/*
**join wifi fun
*/
int esp32_join_wifi()
{
	int rc;
	
	char* pWifissid;
	char* pWifipassword;
	
	pWifissid = (char*)g_fsData.wifissid;
	pWifipassword = (char*)g_fsData.wifipassword;
	
	if(esp32wifi_init_at() !=0)
	{
		return -1;
	}
	
	if(esp32_join_ap(pWifissid, pWifipassword) != 0)
	{
		return -1;
	}
	
	return 0;
	
}
