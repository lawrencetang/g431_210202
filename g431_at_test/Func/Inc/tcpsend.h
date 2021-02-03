/**
  ******************************************************************************
  * @file           : mqttpub.h
  * @brief          : Header for mqttpub.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _TCPSEND_H
#define _TCPSEND_H

#include "sal_module_wrapper.h"
#include "tos_at.h"

#define MAX_TCP_H5GID_STR 32

extern void tcp_init();

extern int tcp_conn(char* ip, char* port);

extern int tcp_send_sersonTd();

extern int tcp_close();

typedef struct _tcpSend
{
	int socketId;
	uint8_t h5gecId[MAX_TCP_H5GID_STR + 1];
	uint32_t startTime;
	uint32_t lastTime;
	float tempValue;
	float* pvoiceValue;
	float* pxvibValue;
	float* pyvibValue;
	float* pzvibValue;
}tcpSend;

#endif
