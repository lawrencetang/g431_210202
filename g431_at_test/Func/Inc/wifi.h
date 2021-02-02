/**
  ******************************************************************************
  * @file           : wifi.h
  * @brief          : Header for wifi.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _WIFI_H
#define _WIFI_H

#include "sal_module_wrapper.h"
//#include "esp8266.h"
#include "tos_at.h"

extern void esp32_test_wifi();
extern int esp32_join_wifi();

#define WIFI_RECV_LEN            1024


#endif
