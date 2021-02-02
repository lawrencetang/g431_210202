/**
  ******************************************************************************
  * @file           : bsp.h
  * @brief          : Header for bsp.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _BSP_H
#define _BSP_H

#include "cmsis_os.h"

//extern k_sem_t sem_tsample;
extern k_sem_t sem_mqttrev_fin;
extern k_sem_t sem_mqttrev_en;

/**********ev_tsample begin**********/
extern k_event_t ev_tsample;
extern const k_event_flag_t ev_rtc_tsample;
extern const k_event_flag_t ev_wake_tsample;
/**********ev_tsample end**********/

/**********ev_nsample begin**********/
extern k_event_t ev_nsample;
extern const k_event_flag_t ev_rtc_nsample;
extern const k_event_flag_t ev_wake_nsample;
/**********ev_nsample end**********/

/**********ev_vsample begin**********/
extern k_event_t ev_vsample;
extern const k_event_flag_t ev_rtc_vsample;
extern const k_event_flag_t ev_wake_vsample;
/**********ev_vsample end**********/

/**********ev_mqttpub begin**********/
extern k_event_t ev_mqttpub;
extern const k_event_flag_t ev_normal_pub;
extern const k_event_flag_t ev_exce_pub;
extern const k_event_flag_t ev_para_pub;
/**********ev_mqttpub end**********/

/**********ev_samplefin begin**********/
extern k_event_t ev_samplefin;
extern const k_event_flag_t ev_tsamplefin;
extern const k_event_flag_t ev_nsamplefin;
extern const k_event_flag_t ev_vsamplefin;
/**********ev_samplefin end**********/

/**********sem_samplefin begin**********/
extern k_sem_t sem_sample_mqttfin;
/**********sem_samplefin end**********/

/**********sem_tsamplemqtt begin**********/
extern k_sem_t sem_tsample_mqtt;
/**********sem_tsamplemqtt end**********/

/**********sem_nsamplemqtt begin**********/
extern k_sem_t sem_nsample_mqtt;
/**********sem_nsamplemqtt end**********/

/**********sem_vsamplemqtt begin**********/
extern k_sem_t sem_vsample_mqtt;
/**********sem_vsamplemqtt end**********/

/**********unixtime begin**********/
typedef struct _unixTime
{
	int mqtt_unix_st;
  int mqtt_unix_et;

  int tcp_unix_st;
  int tcp_unix_et;
}unixTime;

extern unixTime g_unixTime;
/**********unixtime end**********/

extern void DevInit();
#endif
