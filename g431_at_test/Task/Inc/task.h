/**
  ******************************************************************************
  * @file           : task.h
  * @brief          : Header for task.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
#ifndef _TASK_H
#define _TASK_H

#include "cmsis_os.h"

#define START_NOT_OVER	0x00
#define START_OVER		0x01

#define MQTT_NOT_OVER 0x00
#define MQTT_OVER 0X01

#define TCP_NOT_OVER 0x00
#define TCP_OVER 0x01

extern uint8_t g_u8StartOver;
extern uint8_t g_u8MqttOver;
extern uint8_t g_u8TcpOver;

/*********TASK_MAIN BEGIN***********/

#define STK_SIZE_TASK_MAIN      1024
#define TASK_MAIN_PRIO      4

extern k_task_t task_main;
extern k_stack_t stack_task_main[STK_SIZE_TASK_MAIN];
extern k_sem_t uart3_rec_sem;

extern void task_main_entry(void *arg);

/*********TASK_MAIN END***********/

/*********PM_TASK BEGIN***********/
#define STK_SIZE_PM_TASK      128
#define PM_TASK_PRIO      5

extern k_task_t pm_task;
extern k_stack_t stack_pm_task[STK_SIZE_PM_TASK];

extern void pm_task_entry(void *arg);
/*********PM_TASK END***********/

/*********LED_TASK BEGIN***********/
#define STK_SIZE_LED_TASK      128
#define LED_TASK_PRIO      5

extern k_task_t led_task;
extern k_stack_t stack_led_task[STK_SIZE_LED_TASK];

extern void led_task_entry(void *arg);
/*********LED_TASK END***********/

/*********TSAMPLE_TASK BEGIN***********/
#define STK_SIZE_TSAMPLE_TASK      512
#define TSAMPLE_TASK_PRIO      6

extern k_task_t tsample_task;
extern k_stack_t stack_tsample_task[STK_SIZE_TSAMPLE_TASK];

extern void tsample_task_entry(void *arg);
/*********TSAMPLE_TASK END***********/

/*********NSAMPLE_TASK BEGIN***********/
#define STK_SIZE_NSAMPLE_TASK      512
#define NSAMPLE_TASK_PRIO      6

extern k_task_t nsample_task;
extern k_stack_t stack_nsample_task[STK_SIZE_NSAMPLE_TASK];

extern void nsample_task_entry(void *arg);
/*********NSAMPLE_TASK END***********/

/*********VSAMPLE_TASK BEGIN***********/
#define STK_SIZE_VSAMPLE_TASK      512
#define VSAMPLE_TASK_PRIO      6

extern k_task_t vsample_task;
extern k_stack_t stack_vsample_task[STK_SIZE_VSAMPLE_TASK];

extern void vsample_task_entry(void *arg);
/*********VSAMPLE_TASK END***********/

/*********MQTTSUB_TASK BEGIN***********/
#define STK_SIZE_MQTTSUB_TASK      1024
#define MQTTSUB_TASK_PRIO      3

extern k_task_t mqttsub_task;
extern k_stack_t stack_mqttsub_task[STK_SIZE_MQTTSUB_TASK];

extern void mqttsub_task_entry(void *arg);
/*********MQTTSUB_TASK END***********/

/*********MQTTPUB_TASK BEGIN***********/
#define STK_SIZE_MQTTPUB_TASK      2048
#define MQTTPUB_TASK_PRIO      3

extern k_task_t mqttpub_task;
extern k_stack_t stack_mqttpub_task[STK_SIZE_MQTTPUB_TASK];

extern void mqttpub_task_entry(void *arg);
/*********MQTTPUB_TASK END***********/

/*********TCPSEND_TASK BEGIN***********/
#define STK_SIZE_TCPSEND_TASK      512
#define TCPSEND_TASK_PRIO      7

extern k_task_t tcpsend_task;
extern k_stack_t stack_tcpsend_task[STK_SIZE_TCPSEND_TASK];

extern void tcpsend_task_entry(void *arg);
/*********TCPSEND_TASK END***********/

extern int list_mmheap_info(void);

#endif
