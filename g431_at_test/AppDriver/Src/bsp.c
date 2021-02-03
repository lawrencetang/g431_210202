/**
  ******************************************************************************
  * @file           : bsp.c
  * @brief          : bsp program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "bsp.h"
#include "fs.h"
#include "mqttsub.h"
#include "mqttpub.h"
#include "tcpsend.h"

k_sem_t sem_mqttrev_fin;
k_sem_t sem_mqttrev_en;

/**********ev_tsample begin**********/
k_event_t ev_tsample;
const k_event_flag_t ev_rtc_tsample     = (k_event_flag_t)(1 << 0);
const k_event_flag_t ev_wake_tsample     = (k_event_flag_t)(1 << 1);
/**********ev_tsample end**********/

/**********ev_nsample begin**********/
k_event_t ev_nsample;
const k_event_flag_t ev_rtc_nsample     = (k_event_flag_t)(1 << 0);
const k_event_flag_t ev_wake_nsample     = (k_event_flag_t)(1 << 1);
/**********ev_nsample end**********/

/**********ev_vsample begin**********/
k_event_t ev_vsample;
const k_event_flag_t ev_rtc_vsample     = (k_event_flag_t)(1 << 0);
const k_event_flag_t ev_wake_vsample     = (k_event_flag_t)(1 << 1);
/**********ev_vsample end**********/

/**********ev_mqttpub begin**********/
k_event_t ev_mqttpub;
const k_event_flag_t ev_normal_pub     = (k_event_flag_t)(1 << 0);
const k_event_flag_t ev_exce_pub     = (k_event_flag_t)(1 << 1);
const k_event_flag_t ev_para_pub     = (k_event_flag_t)(1 << 2);
/**********ev_mqttpub end**********/

/**********ev_samplefin begin**********/
k_event_t ev_samplefin;
const k_event_flag_t ev_tsamplefin     = (k_event_flag_t)(1 << 0);
const k_event_flag_t ev_nsamplefin     = (k_event_flag_t)(1 << 1);
const k_event_flag_t ev_vsamplefin     = (k_event_flag_t)(1 << 2);
/**********ev_samplefin end**********/

/**********sem_sample_mqttfin begin**********/
k_sem_t sem_sample_mqttfin;
/**********sem_samplefin end**********/

/**********sem_tsamplemqtt begin**********/
k_sem_t sem_tsample_mqtt;
/**********sem_tsamplemqtt end**********/

/**********sem_nsamplemqtt begin**********/
k_sem_t sem_nsample_mqtt;
/**********sem_nsamplemqtt end**********/

/**********sem_vsamplemqtt begin**********/
k_sem_t sem_vsample_mqtt;
/**********sem_vsamplemqtt end**********/

/**********unixtime begin**********/
unixTime g_unixTime;
/**********unixtime end**********/

/**********sample_tcpfin begin**********/
uint8_t g_ttcpfin_flag = 0;
uint8_t g_ntcpfin_flag = 0;
uint8_t g_vtcpfin_flag = 0;
/**********sample_tcpfin end**********/

void TimeInit()
{
	g_unixTime.mqtt_unix_et = 0;
	g_unixTime.mqtt_unix_st = 0;
	g_unixTime.tcp_unix_et = 0;
	g_unixTime.tcp_unix_st = 0;
}

void SemInit()
{
	tos_sem_create(&sem_mqttrev_fin, 0);
	tos_sem_create(&sem_mqttrev_en, 0);
	tos_sem_create(&sem_sample_mqttfin, 0);
	tos_sem_create(&sem_tsample_mqtt, 0);
	tos_sem_create(&sem_nsample_mqtt, 0);
	tos_sem_create(&sem_vsample_mqtt, 0);
}

void EventInit()
{
	tos_event_create(&ev_tsample, (k_event_flag_t)0u);
	tos_event_create(&ev_nsample, (k_event_flag_t)0u);
	tos_event_create(&ev_vsample, (k_event_flag_t)0u);
	
	tos_event_create(&ev_mqttpub, (k_event_flag_t)0u);
	
	tos_event_create(&ev_samplefin, (k_event_flag_t)0u);
}

void GpioInit()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
}

void DevInit()
{
	fs_init();
	mqttsub_init();
	mqttpub_init();
	tcp_init();
	SemInit();
	EventInit();
	GpioInit();
	TimeInit();
}
