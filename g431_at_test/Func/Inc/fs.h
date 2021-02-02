/**
  ******************************************************************************
  * @file           : fs.h
  * @brief          : Header for fs.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
	
#ifndef _FS_H
#define _FS_H

#include "sal_module_wrapper.h"
#include "tos_at.h"
#include "cmsis_os.h"

#include "stdio.h"
#include "stdbool.h"
#include "ctype.h"

#define CONFIGE_FILE "conf_file"

#define MAX_WIFISSID_STRING 32
#define MAX_WIFIPASSWORD_STRING 32
#define MAX_DEVGID_STRING 32
#define MAX_DEVUUID_STRING 32
#define MAX_MQCLIENTID_STRING 32
#define MAX_MQUSERNAME_STRING 64
#define MAX_MQPASSWORD_STRING 128

typedef struct _fsData
{
	uint8_t wifissid[MAX_WIFISSID_STRING];
	uint8_t wifipassword[MAX_WIFIPASSWORD_STRING];
	uint8_t devgid[MAX_DEVGID_STRING];
	uint8_t devuuid[MAX_DEVUUID_STRING];
	uint8_t mqclientid[MAX_MQCLIENTID_STRING];
	uint8_t mqusername[MAX_MQUSERNAME_STRING];
	uint8_t mqpassword[MAX_MQPASSWORD_STRING];
}fsData;

extern fsData g_fsData;

extern void fs_init();
extern int fs_read_file(char* filename);

#endif