/**
  ******************************************************************************
  * @file           : ble.h
  * @brief          : Header for ble.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _BLE_H
#define _BLE_H

#include "sal_module_wrapper.h"
#include "tos_at.h"
#include "cmsis_os.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

extern int esp32ble_sal_init(hal_uart_port_t uart_port);
extern int esp32ble_init_at(void);

extern int ble_resolve_para(char* conf_data);

#endif