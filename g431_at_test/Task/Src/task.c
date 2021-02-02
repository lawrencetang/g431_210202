/**
  ******************************************************************************
  * @file           : task.c
  * @brief          : task program body
  ******************************************************************************
  */

#include "stm32g4xx_hal.h"
#include "task.h"

#include "wifi.h"
#include "bsp.h"
#include "ble.h"
#include "ntp.h"
#include "esp32.h"
#include "itcallback.h"

uint8_t g_u8StartOver = START_NOT_OVER;
uint8_t g_u8MqttOver = MQTT_NOT_OVER;

/*********TASK_MAIN BEGIN***********/
k_task_t task_main;
k_stack_t stack_task_main[STK_SIZE_TASK_MAIN]; 

k_sem_t uart3_rec_sem;

void task_main_entry(void *arg)
{
	//lt:��ʼ�����ݽṹ
	k_err_t err;
	k_err_t fin_err;
	k_event_flag_t flag_match;
	
	printf("\r\n###access task_main_entry\r\n");
	DevInit();
	g_u8StartOver = START_NOT_OVER;
	tos_sem_create(&uart3_rec_sem, 0);
	while(1)
	{
		err = tos_sem_pend(&uart3_rec_sem, TOS_TIME_FOREVER);
	  if (err == K_ERR_NONE)
	  {
		  //lt:����������,��ջ���
		  printf("get uart3 data %s\n", uart3cach_data);
		  if(ble_resolve_para((char*)uart3cach_data) == 0)
		  {
				//lt:�����ɹ�,break
				printf("resolve ble data success\n");
				memset(uart3cach_data, 0, 512);
				break;
		  }
		  memset(uart3cach_data, 0, 512);
	  }
	}
	
	//lt:����uart3_rec_sem�ź���
	tos_sem_destroy(&uart3_rec_sem);
	
	//lt:������������ȴ�wifi���ӵ�����
//	(void)tos_task_create(&pm_task, "pm_task", pm_task_entry,
//                          K_NULL, PM_TASK_PRIO,
//                          stack_pm_task, STK_SIZE_PM_TASK, 0);
	(void)tos_task_create(&led_task, "led_task", led_task_entry,
                          K_NULL, LED_TASK_PRIO,
                          stack_led_task, STK_SIZE_LED_TASK, 0);
	//lt:�����¶Ȳ���task
	(void)tos_task_create(&tsample_task, "tsample_task", tsample_task_entry,
                          K_NULL, TSAMPLE_TASK_PRIO,
                          stack_tsample_task, STK_SIZE_TSAMPLE_TASK, 0);
	
	//lt:������������task
	(void)tos_task_create(&nsample_task, "nsample_task", nsample_task_entry,
                          K_NULL, NSAMPLE_TASK_PRIO,
                          stack_nsample_task, STK_SIZE_NSAMPLE_TASK, 0);
	
	//lt:�����𶯲���task
	(void)tos_task_create(&vsample_task, "vsample_task", vsample_task_entry,
                          K_NULL, VSAMPLE_TASK_PRIO,
                          stack_vsample_task, STK_SIZE_VSAMPLE_TASK, 0);
	
	//lt:����mqtt���Ӷ���task
	(void)tos_task_create(&mqttsub_task, "mqttsub_task", mqttsub_task_entry,
                          K_NULL, MQTTSUB_TASK_PRIO,
                          stack_mqttsub_task, STK_SIZE_MQTTSUB_TASK, 0);
	
	//lt:����mqtt����task
	(void)tos_task_create(&mqttpub_task, "mqttpub_task", mqttpub_task_entry,
                          K_NULL, MQTTPUB_TASK_PRIO,
                          stack_mqttpub_task, STK_SIZE_MQTTPUB_TASK, 0);
	printf("task create complete\n" );
	//list_mmheap_info();
	//lt:��ʼ��esp32
	if(esp32_sal_init(HAL_UART_PORT_1) != 0) //ltg
	{
		printf("Init ESP32ble failed\n" );
		return;
	}
	//list_mmheap_info();
	//lt:��ʼ����
	if(esp32ble_init_at() == -1)
	{
		printf("ble error\n");
	}
	//lt:��ʼ���������ļ�
	
	//lt:����wifi,���ɹ����5sһֱ��������
	while(esp32_join_wifi() != 0)
	{
		tos_task_delay(5000);
	}
	printf("wifi connect success\n");
	
	//lt:���ò���ʱ��
	if(set_sample_time() != 0)
	{
		printf("set sample time failed\n");
	}
	
	g_u8StartOver = START_OVER;
	
	while(1)
  {
		//lt:���mqtt�ɼ��¼�,������������mqtt�ɼ��¼���ɺ�֪ͨmqttpubtaskȥ����,����������RTC��ʱ
//    printf("\r\n###This is taskmain\r\n");
//    tos_task_delay(2000);
		fin_err = tos_sem_pend(&sem_sample_mqttfin, TOS_TIME_FOREVER);
		if (fin_err == K_ERR_NONE)
		{
			fin_err = tos_sem_pend(&sem_sample_mqttfin, TOS_TIME_FOREVER);
			if (fin_err == K_ERR_NONE)
			{
				fin_err = tos_sem_pend(&sem_sample_mqttfin, TOS_TIME_FOREVER);
				if (fin_err == K_ERR_NONE)
				{
					//lt:mqtt�ɷ���
					printf("\r\n###3 sensor mqtt fin\r\n");
					//lt:�����ϱ�
					tos_event_post(&ev_mqttpub, ev_normal_pub);
				}
			}
		}
  }
	return;
}

/*********TASK_MAIN END***********/

/**
 *@brief   ��ӡ��ǰmmeheapʹ�����
 *@param   none
 *@retval  none
*/
int list_mmheap_info(void)
{
    k_err_t err;
    k_mmheap_info_t mmheap_info;

    err = tos_mmheap_check(&mmheap_info);
    if (err != K_ERR_NONE) {
        printf("current mmheap info check fail, err = %d\r\n", err);
        return -1;
    }
    else {
        printf("current mmheap info:\r\n\tused: %d[0x%08x] free:%d[0x%08x]\r\n\r\n", mmheap_info.used, mmheap_info.used, mmheap_info.free, mmheap_info.free);
        return 0;
    }
}


