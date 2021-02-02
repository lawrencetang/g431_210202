/**
  ******************************************************************************
  * @file           : esp32.c
  * @brief          : esp32 program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "esp32.h"

#include "tos_at.h"
//#include "esp8266.h"
#include "sal_module_wrapper.h"

#include "mqttsub.h"
#include "bsp.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

#define MAX_MQTT_REV_LEN 2048

int esp32_restore(void)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 3000, "AT+RESTORE\r\n");
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

int esp32_echo_close(void)
{
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    tos_at_cmd_exec(&echo, 1000, "ATE0\r\n");
    if (echo.status == AT_ECHO_STATUS_OK) {
        return 0;
    }
    return -1;
}

int esp32_net_mode_set(esp32_net_mode_t mode)
{
    int try = 0;
    char *cmd = NULL;
    at_echo_t echo;

    switch (mode) {
        case ESP32_NET_MODE_STA:
            cmd = "AT+CWMODE=1\r\n";
            break;
        case ESP32_NET_MODE_AP:
            cmd = "AT+CWMODE=2\r\n";
            break;
        case ESP32_NET_MODE_STA_AP:
            cmd = "AT+CWMODE=3\r\n";
            break;
        default:
            return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "no change");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, cmd);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}

int esp32_send_mode_set(esp32_send_mode_t mode)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, NULL);
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 1000, "AT+CIPMODE=%d\r\n", mode == ESP32_SEND_MODE_NORMAL ? 0 : 1);
        if (echo.status == AT_ECHO_STATUS_OK) {
            return 0;
        }
    }
    return -1;
}

int esp32_multilink_set(esp32_multilink_state_t state)
{
    int try = 0;
    at_echo_t echo;

    tos_at_echo_create(&echo, NULL, 0, "link is builded");
    while (try++ < 10) {
        tos_at_cmd_exec(&echo, 500, "AT+CIPMUX=%d\r\n", state == ESP32_MULTILINK_STATE_ENABLE ? 1 : 0);
        if (echo.status == AT_ECHO_STATUS_OK || echo.status == AT_ECHO_STATUS_EXPECT) {
            return 0;
        }
    }
    return -1;
}


__STATIC__ void esp32_incoming_data_process(void)
{
}

__STATIC__ void esp32_mqttsub_data_process(void)
{
	uint8_t data;
	k_err_t err;
	
	err = tos_sem_pend(&sem_mqttrev_en, TOS_TIME_FOREVER);
	if(err == K_ERR_NONE)
	{
		g_mqttRevData.pmq_rev_data = (uint8_t*)tos_mmheap_alloc(MAX_MQTT_REV_LEN + 1);
		memset(g_mqttRevData.pmq_rev_data, 0 , MAX_MQTT_REV_LEN + 1);
	  g_mqttRevData.mq_rev_len = 0;
		
		//lt:topic message
	  while(1)
	  {
		  if (tos_at_uart_read(&data, 1) != 1)
		  {
			  return;
			}
		  
		  if(g_mqttRevData.mq_rev_len == MAX_MQTT_REV_LEN)
		  {
			  break;
		  }
		  if (data == '\r')
      {
        break;
      }
		  g_mqttRevData.pmq_rev_data[g_mqttRevData.mq_rev_len++] = data;
		}
		
		/* 读取'\r'之后的'\n'，不作任何处理 */
    if (tos_at_uart_read(&data, 1) != 1)
    {
        return;
    }
	
	  //发信号去mqttsubtask去读取
	  tos_sem_post(&sem_mqttrev_fin);
	}
}

at_event_t esp32_at_event[] = {
    { "+IPD,", esp32_incoming_data_process },
		{ "+MQTTSUBRECV:", esp32_mqttsub_data_process },
};

static int esp32_init(void)
{
	printf("Init ESP32 ...\n" );
	
	printf("Init ESP32 Done\n" );
  return 0;
	//lt:暂时不写初始化的东西,先让其走注册的流程
}

/*
    if we have got wifi unconnected once before, and now we get connected
    to ap again, we should set echo close and single/multilink again
 */
static int esp32_reconnect_init(void)
{
	  /*
    if (esp32_echo_close() != 0) {
        printf("esp32 echo close FAILED\n");
        return -1;
    }
	*/

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

    return 0;
}

static int esp32_connect(const char *ip, const char *port, sal_proto_t proto)
{
	  int id;
    at_echo_t echo;

    esp32_reconnect_init();

    id = tos_at_channel_alloc(ip, port);
    if (id == -1) {
        return -1;
    }

    tos_at_echo_create(&echo, NULL, 0, "OK");
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec_until(&echo, 10000,
                        "AT+CIPSTART=\"%s\",\"%s\",%s\r\n",
                        proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
#else
    tos_at_cmd_exec_until(&echo, 10000,
                        "AT+CIPSTART=%d,\"%s\",\"%s\",%s\r\n",
                        id, proto == TOS_SAL_PROTO_UDP ? "UDP" : "TCP", ip, port);
#endif
    if (echo.status == AT_ECHO_STATUS_EXPECT) {
        return id;
    }

    tos_at_channel_free(id);

    return -1;
}

static int esp32_parse_domain(const char *host_name, char *host_ip, size_t host_ip_len)
{
    char *str;
    at_echo_t echo;
    char echo_buffer[64];

    esp32_reconnect_init();

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), NULL);
    tos_at_cmd_exec(&echo, 2000, "AT+CIPDOMAIN=\"%s\"\r\n", host_name);

    if (echo.status != AT_ECHO_STATUS_OK) {
        return -1;
    }

    /*
    +CIPDOMAIN:183.232.231.172
     */
    str = strstr((const char *)echo.buffer, "+CIPDOMAIN:");
    if (!str) {
        return -1;
    }
    sscanf(str, "+CIPDOMAIN:%s", host_ip);
    host_ip[host_ip_len - 1] = '\0';
    printf("GOT IP: %s\n", host_ip);
    return 0;
}

static int esp32_close(int id)
{
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE\r\n");
#else
    tos_at_cmd_exec(NULL, 1000, "AT+CIPCLOSE=%d\r\n", id);
#endif
    tos_at_channel_free(id);
    return 0;
}

static int esp32_recv_timeout(int id, void *buf, size_t len, uint32_t timeout)
{
    return tos_at_channel_read_timed(id, buf, len, timeout);
}

static int esp32_recv(int id, void *buf, size_t len)
{
    return esp32_recv_timeout(id, buf, len, (uint32_t)4000);
}

static int esp32_is_link_broken(const char *echo_buffer)
{
    if (strstr(echo_buffer, "CLOSED") ||
        strstr(echo_buffer, "link is not valid")) {
        return 1;
    }

    return 0;
}

static int esp32_send(int id, const void *buf, size_t len)
{
    at_echo_t echo;
    char echo_buffer[64];

    if (!tos_at_channel_is_working(id)) {
        return -1;
    }

    if (tos_at_global_lock_pend() != 0) {
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), ">");
#if TOS_CFG_MODULE_SINGLE_LINK_EN > 0u
    tos_at_cmd_exec(&echo, 1000,
                        "AT+CIPSEND=%d\r\n", len);
#else
    tos_at_cmd_exec(&echo, 1000,
                        "AT+CIPSEND=%d,%d\r\n",
                        id, len);
#endif
    if (echo.status != AT_ECHO_STATUS_OK && echo.status != AT_ECHO_STATUS_EXPECT) {
        if (esp32_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_echo_create(&echo, echo_buffer, sizeof(echo_buffer), "SEND OK");

    /*
        ATTENTION: we should wait util "SEND OK" is echoed, otherwise the next
        time we execute at command, the esp8266 maybe in a "busy s ..." state.
     */
    tos_at_raw_data_send_until(&echo, 10000, (uint8_t *)buf, len);
    if (echo.status != AT_ECHO_STATUS_EXPECT) {
        if (esp32_is_link_broken((const char *)echo.buffer)) {
            tos_at_channel_set_broken(id);
        }

        tos_at_global_lock_post();
        return -1;
    }

    tos_at_global_lock_post();
    return len;
}


sal_module_t sal_module_esp32 = {
    .init               = esp32_init,
	  .connect            = esp32_connect,
	  .send               = esp32_send,
	  .recv_timeout       = esp32_recv_timeout,
	  .recv               = esp32_recv,
	  .close              = esp32_close,
	  .parse_domain       = esp32_parse_domain,
    //.connect            = esp8266_connect,
    //.send               = esp8266_send,
    //.recv_timeout       = esp8266_recv_timeout,
    //.recv               = esp8266_recv,
    //.sendto             = esp8266_sendto,
    //.recvfrom           = esp8266_recvfrom,
    //.recvfrom_timeout   = esp8266_recvfrom_timeout,
    //.close              = esp8266_close,
    //.parse_domain       = esp8266_parse_domain,
};


int esp32_sal_init(hal_uart_port_t uart_port)
{
	  if (tos_at_init(uart_port, esp32_at_event,
                        sizeof(esp32_at_event) / sizeof(esp32_at_event[0])) != 0) {
        return -1;
    }										

    if (tos_sal_module_register(&sal_module_esp32) != 0) {
        return -1;
    }

    if (tos_sal_module_init() != 0) {
        return -1;
    }

    return 0;
}
