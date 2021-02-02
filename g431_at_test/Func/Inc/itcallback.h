/**
  ******************************************************************************
  * @file           : itcallback.c
  * @brief          : interupt program body
  ******************************************************************************
  */
#ifndef _ITCALLBACK_H
#define _ITCALLBACK_H

#include "stm32g4xx_hal.h"

extern uint8_t uart1recv_data;
extern uint8_t uart3recv_data;
extern uint8_t uart3itcount;

extern uint8_t uart3cach_data[];

#endif
