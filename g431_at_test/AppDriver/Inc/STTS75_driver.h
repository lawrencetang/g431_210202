#ifndef __STTS75_H
#define __STTS75_H

#include "stm32g4xx.h"
#include "i2c.h"
#include "stdio.h"

#ifndef __ARCHDEP__TYPES
#define __ARCHDEP__TYPES

typedef unsigned char u8_t;
typedef unsigned short int u16_t;
typedef unsigned int u32_t;
typedef signed int i32_t;
typedef signed short int i16_t;
typedef signed char i8_t;

#endif /*__ARCHDEP__TYPES*/



typedef enum {
  STTS_SUCCESS				=		0x01,
  STTS_ERROR				=		0x00	
} stts_status_t;


/*****************user code begin**********************/


typedef enum {
	STTS_ERROR_NONE						=		0x00,
	STTS_ERROR_HARD_WARE			=		0x01,	//硬件故障
	STTS_ERROR_READ_DATA			= 	0x02	//温度读取失败
} stts_err_t;

/*****************user code end**********************/


/************** STTS75 I2C Address *****************/
/*	7位地址： 1001xxx b
*	取决于A0、A1、A2接线方式
*	比如：A0-2 都接地，地址为1001000------0x48
*/
#define STTS75_I2C_ADDRESS_READ     ((0x48<<1) | 0x01)	
#define STTS75_I2C_ADDRESS_WRITE    (0x48<<1)

/************** STTS75 寄存器地址 *****************/
#define STTS75_I2C_TEMP		0x00
#define STTS75_I2C_CONF		0x01
#define STTS75_I2C_THYS		0x02
#define STTS75_I2C_TOS		0x03

/********************* 常用函数 ***********************/
stts_status_t STTS75_Check(void *handle, u8_t DeviceAddress);
stts_status_t STTS75_GET_Temp(void *handle, float *Value);

/*******************************************************************************
* Register      : TEMP
* Address       : 0x00
* Bit Group Name: Temperature register
* Permission    : R
* Default		: N/A    0xC900~0x7D00
*******************************************************************************/
#define  	STTS75_I2C_TEMP_BIT_SB  		0x8000
#define  	STTS75_I2C_TEMP_BIT_POSITION  	15
stts_status_t STTS75_R_TEMP_BIT_SB(void *handle, u16_t *value);


/*******************************************************************************
* Register      : CONF
* Address       : 0x01
* Bit Group Name: Configuration register
* Permission    : RW
* Default		: 0x00
*******************************************************************************/
typedef enum {
	STTS75_CONF_RC_1			= 0x00,	// 0.5 °C
	STTS75_CONF_RC_2			= (0x01<<5),	// 0.25 °C
	STTS75_CONF_RC_3			= (0x02<<5),	// 0.125 °C
	STTS75_CONF_RC_4			= (0x03<<5)	// 0.0625 °C
} STTS75_CONF_RC_t;

#define  	STTS75_CONF_RC_MASK  	(0x03<<5)
stts_status_t STTS75_W_Resolution(void *handle, STTS75_CONF_RC_t newValue);
stts_status_t STTS75_R_Resolution(void *handle, STTS75_CONF_RC_t *value);


/*******************************************************************************
* Register      : THYS
* Address       : 0x02
* Bit Group Name: Hysteresis register
* Permission    : RW
* Default		: 0x4800
* Description	: Defaults to 75 °C at power-up (i.e., 0100 1011 0000 0000)
*******************************************************************************/
#define  	STTS75_THYS_DEF  		0x4B00
stts_status_t STTS75_W_Thys(void *handle, u16_t newValue);
stts_status_t STTS75_R_Thys(void *handle, u16_t *value);

/*******************************************************************************
* Register      : TOS
* Address       : 0x03
* Bit Group Name: Overtemperature shutdown
* Permission    : RW
* Default		: 0x5000
* Description	: Defaults to 80 °C at power-up (i.e., 0101 0000 0000 0000)
*******************************************************************************/
#define  	STTS75_TOS_DEF  		0x5000
stts_status_t STTS75_W_Tos(void *handle, u16_t newValue);
stts_status_t STTS75_R_Tos(void *handle, u16_t *value);




#endif /*__STTS75_H*/



