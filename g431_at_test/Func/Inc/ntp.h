/**
  ******************************************************************************
  * @file           : ntp.h
  * @brief          : Header for ntp.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _NTP_H
#define _NTP_H

#include "sal_module_wrapper.h"
#include "tos_at.h"
#include "cmsis_os.h"

extern int set_sample_time();
extern int again_set_sample_time();
extern int get_unix_time(int* time_value);

#endif