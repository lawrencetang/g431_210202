/**
  ******************************************************************************
  * @file           : ntp.c
  * @brief          : ntp program body
  ******************************************************************************
  */
	
#include "stm32g4xx_hal.h"
#include "ntp.h"
#include "esp32.h"
#include "rtc.h"
#include "include.h"


#define MAX_TIME_STR 64
#define NTP_TIME_OFFSET (13)
#define REPEAT_SMAPLE_TIME (15)

static int esp32_ntp_set(void)
{
	at_echo_t echo;

  tos_at_echo_create(&echo, NULL, 0, NULL);
  tos_at_cmd_exec(&echo, 1000, "AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\",\"ntp.sjtu.edu.cn\"\r\n");
  if (echo.status == AT_ECHO_STATUS_OK) {
      return 0;
  }
  return -1;
}

static int esp32_ntp_get(char* time_str, uint8_t len)
{
	int try = 0;
	at_echo_t echo;
	char* str;
	
	tos_at_echo_create(&echo, time_str, len, NULL);
	
	while(try++ < 10)
	{
		tos_at_cmd_exec(&echo, 5000, "AT+CIPSNTPTIME?\r\n");
		
		if (echo.status == AT_ECHO_STATUS_OK)
    {
			str = strstr(echo.buffer, "+CIPSNTPTIME:");
			memcpy(time_str, str, strlen(str));
      return 0;
    }
	}
	return -1;
}

static int String2BcdTime(char* time_str, uint8_t* time_bcd, struct tm *time_tm)
{
	char month[4] = {0};
	
	if(time_str[8 + NTP_TIME_OFFSET] == ' ')
	{
		time_str[8 + NTP_TIME_OFFSET] = '0';
	}
	
	memcpy(month, &time_str[4 + NTP_TIME_OFFSET], 3);
	time_bcd[0] = (uint8_t)((((time_str[8 + NTP_TIME_OFFSET] - '0')&0x0f) << 4) + ((time_str[9 + NTP_TIME_OFFSET] - '0')&0x0f)); //lt:日
	time_bcd[1] = (uint8_t)((((time_str[11 + NTP_TIME_OFFSET] - '0')&0x0f) << 4) + ((time_str[12 + NTP_TIME_OFFSET] - '0')&0x0f)); //lt:时
	time_bcd[2] = (uint8_t)((((time_str[14 + NTP_TIME_OFFSET] - '0')&0x0f) << 4) + ((time_str[15 + NTP_TIME_OFFSET] - '0')&0x0f)); //lt:分
	time_bcd[3] = (uint8_t)((((time_str[17 + NTP_TIME_OFFSET] - '0')&0x0f) << 4) + ((time_str[18 + NTP_TIME_OFFSET] - '0')&0x0f)); //lt:秒
	time_bcd[4] = (uint8_t)((((time_str[22 + NTP_TIME_OFFSET] - '0')&0x0f) << 4) + ((time_str[23 + NTP_TIME_OFFSET] - '0')&0x0f)); //lt:年
	
	time_tm->tm_year = (time_str[20 + NTP_TIME_OFFSET] - '0') * 1000 + (time_str[21 + NTP_TIME_OFFSET] - '0') * 100 + (time_str[22 + NTP_TIME_OFFSET] - '0') * 10 
	                   + (time_str[23 + NTP_TIME_OFFSET] - '0') - 1900;
	time_tm->tm_mday = (time_str[8 + NTP_TIME_OFFSET] - '0') * 10 + (time_str[9 + NTP_TIME_OFFSET] - '0');
	time_tm->tm_hour = (time_str[11 + NTP_TIME_OFFSET] - '0') * 10 + (time_str[12 + NTP_TIME_OFFSET] - '0');
	time_tm->tm_min = (time_str[14 + NTP_TIME_OFFSET] - '0') * 10 + (time_str[15 + NTP_TIME_OFFSET] - '0');
	time_tm->tm_sec = (time_str[17 + NTP_TIME_OFFSET] - '0') * 10 + (time_str[18 + NTP_TIME_OFFSET] - '0');
	
//	printf("tm_mday *10 = %d\n", (time_str[8 + NTP_TIME_OFFSET] - '0') * 10);
//	printf("tm_mday *1 = %d\n", (time_str[9 + NTP_TIME_OFFSET] - '0'));
	
	if(!strcmp(month, "Jan"))
	{
		time_bcd[5] = RTC_MONTH_JANUARY;
		time_tm->tm_mon = 0;
	}
	else if(!strcmp(month, "Feb"))
	{
		time_bcd[5] = RTC_MONTH_FEBRUARY;
		time_tm->tm_mon = 1;
	}
	else if(!strcmp(month, "Mar"))
	{
		time_bcd[5] = RTC_MONTH_MARCH;
		time_tm->tm_mon = 2;
	}
	else if(!strcmp(month, "Apr"))
	{
		time_bcd[5] = RTC_MONTH_APRIL;
		time_tm->tm_mon = 3;
	}
	else if(!strcmp(month, "May"))
	{
		time_bcd[5] = RTC_MONTH_MAY;
		time_tm->tm_mon = 4;
	}
	else if(!strcmp(month, "Jun"))
	{
		time_bcd[5] = RTC_MONTH_JUNE;
		time_tm->tm_mon = 5;
	}
	else if(!strcmp(month, "Jul"))
	{
		time_bcd[5] = RTC_MONTH_JULY;
		time_tm->tm_mon = 6;
	}
	else if(!strcmp(month, "Aug"))
	{
		time_bcd[5] = RTC_MONTH_AUGUST;
		time_tm->tm_mon = 7;
	}
	else if(!strcmp(month, "Sep"))
	{
		time_bcd[5] = RTC_MONTH_SEPTEMBER;
		time_tm->tm_mon = 8;
	}
	else if(!strcmp(month, "Oct"))
	{
		time_bcd[5] = RTC_MONTH_OCTOBER;
		time_tm->tm_mon = 9;
	}
	else if(!strcmp(month, "Nov"))
	{
		time_bcd[5] = RTC_MONTH_NOVEMBER;
		time_tm->tm_mon = 10;
	}
	else if(!strcmp(month, "Dec"))
	{
		time_bcd[5] = RTC_MONTH_DECEMBER;
		time_tm->tm_mon = 11;
	}
	else
	{
		return -1;
	}
	return 0;
}

static void Tm2BcdTime(struct tm* time_tm, uint8_t* time_bcd)
{
	time_bcd[4] = (uint8_t)((((((time_tm->tm_year + 1900) % 100) /10)&0x0f) << 4 ) + ((((time_tm->tm_year + 1900) % 100) % 10)&0x0f)); //lt:年
	time_bcd[5] = (uint8_t)(((((time_tm->tm_mon + 1) / 10)&0x0f) << 4) + (((time_tm->tm_mon + 1) % 10)&0x0f)); //lt:月
	time_bcd[0] = (uint8_t)(((((time_tm->tm_mday) / 10)&0x0f) << 4) + (((time_tm->tm_mday) % 10)&0x0f)); //lt:日
	time_bcd[1] = (uint8_t)(((((time_tm->tm_hour) / 10)&0x0f) << 4) + (((time_tm->tm_hour) % 10)&0x0f)); //lt:时
	time_bcd[2] = (uint8_t)(((((time_tm->tm_min) / 10)&0x0f) << 4) + (((time_tm->tm_min) % 10)&0x0f)); //lt:分
	time_bcd[3] = (uint8_t)(((((time_tm->tm_sec) / 10)&0x0f) << 4) + (((time_tm->tm_sec) % 10)&0x0f)); //lt:分
	
	return;
}

static int set_alarm_time(RTC_TimeTypeDef* sTime, RTC_DateTypeDef* sDate, RTC_AlarmTypeDef* sAlarm, uint8_t* time_bcd, struct tm* time_tm)
{
	time_t unix_time;
	
	//char alarm_time_str[MAX_TIME_STR] = {0};
	uint8_t alarm_time_bcd[6] = {0};
	struct tm* palarm_time_tm;
	
	sTime->Hours = time_bcd[1];
	sTime->Minutes = time_bcd[2];
	sTime->Seconds = time_bcd[3];
	sTime->SubSeconds = 0x0;
	sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime->StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
		printf("set time failed\n");
    return -1;
  }
	
	sDate->Year = time_bcd[4];
	sDate->Month = time_bcd[5];
	sDate->Date = time_bcd[0];
	if (HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
		printf("set data failed\n");
    return -1;
  }
	
	//lt:tm转unix时间
	unix_time = mktime(time_tm);
	printf("unix_time1 is %d\n", unix_time);
	
	//lt:+3600s设置重新采样时间
	unix_time += REPEAT_SMAPLE_TIME;
	printf("unix_time2 is %d\n", unix_time);
	
	//lt:设置alarm bcd
	//sprintf(alarm_time_str, "+CIPSNTPTIME:%s", asctime(localtime(&unix_time)));//lt:是否会造成内存泄漏?返回静态函数
	//printf("alarm_time_str is %s\n", alarm_time_str);
	
	palarm_time_tm = localtime(&unix_time);
	
	Tm2BcdTime(palarm_time_tm, alarm_time_bcd);
	
	printf("alarm_year is 0x%02x\n", alarm_time_bcd[4]);
	printf("alarm_month is 0x%02x\n", alarm_time_bcd[5]);
	printf("alarm_data is 0x%02x\n", alarm_time_bcd[0]);
	printf("alarm_hour is 0x%02x\n", alarm_time_bcd[1]);
	printf("alarm_min is 0x%02x\n", alarm_time_bcd[2]);
	printf("alarm_sec is 0x%02x\n", alarm_time_bcd[3]);
	
	sAlarm->AlarmTime.Hours = alarm_time_bcd[1];
	sAlarm->AlarmTime.Minutes = alarm_time_bcd[2];
	sAlarm->AlarmTime.Seconds = alarm_time_bcd[3];
	sAlarm->AlarmTime.SubSeconds = 0x0;
	sAlarm->AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm->AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm->AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm->AlarmDateWeekDay = alarm_time_bcd[0];
  sAlarm->Alarm = RTC_ALARM_A;
	
	if (HAL_RTC_SetAlarm_IT(&hrtc, sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
		printf("HAL_RTC_SetAlarm_IT failed");
    return -1;
  }
}

int get_unix_time(long* time_value)
{
	struct tm time_tm;
	time_t unix_time;
	RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
	
	if(HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		return -1;
	}
	
	if(HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		return -1;
	}
//	printf("bin year is %d\n", sDate.Year);
//	printf("bin mon is %d\n", sDate.Month);
//	printf("bin day is %d\n", sDate.Date);
//	
//	printf("bin hour is %d\n", sTime.Hours);
//	printf("bin min is %d\n", sTime.Minutes);
//	printf("bin sec is %d\n", sTime.Seconds);
	
	time_tm.tm_year = sDate.Year + 2000 - 1900;
	time_tm.tm_mon = sDate.Month - 1;
	time_tm.tm_mday = sDate.Date;
	
	time_tm.tm_hour = sTime.Hours;
	time_tm.tm_min = sTime.Minutes;
	time_tm.tm_sec = sTime.Seconds;
	
	unix_time = mktime(&time_tm);
	printf("unix_time1 is %d\n", unix_time);
	
	*time_value = (long)unix_time;
	
	return 0;
	
}

int set_sample_time()
{
	char time_str[MAX_TIME_STR] = {0};
	uint8_t time_bcd[6] = {0};
	struct tm time_tm;
	
	
	RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};
	memset(&time_tm, 0, sizeof(time_tm));
	
	if(esp32_ntp_set() != 0)
	{
		printf("set net time failed\n");
		return -1;
	}
	
	if(esp32_ntp_get(time_str, MAX_TIME_STR) != 0)
	{
		printf("get ntp time failed\n");
		return -1;
	}
	
	tos_task_delay(1000);
	
	if(esp32_ntp_get(time_str, MAX_TIME_STR) != 0)
	{
		printf("get ntp time failed\n");
		return -1;
	}
	
	tos_task_delay(1000);
	
	if(esp32_ntp_get(time_str, MAX_TIME_STR) != 0)
	{
		printf("get ntp time failed\n");
		return -1;
	}
	
	printf("now time is %s\n", time_str);
	
	if(String2BcdTime(time_str, time_bcd, &time_tm) != 0)
	{
		printf("strbcdtime failed");
		return -1;
	}
	
	printf("year is 0x%02x\n", time_bcd[4]);
	printf("month is 0x%02x\n", time_bcd[5]);
	printf("data is 0x%02x\n", time_bcd[0]);
	printf("hour is 0x%02x\n", time_bcd[1]);
	printf("min is 0x%02x\n", time_bcd[2]);
	printf("sec is 0x%02x\n", time_bcd[3]);
	
	printf("year is %d\n", time_tm.tm_year);
	printf("month is %d\n", time_tm.tm_mon);
	printf("data is %d\n", time_tm.tm_mday);
	printf("hour is %d\n", time_tm.tm_hour);
	printf("min is %d\n", time_tm.tm_min);
	printf("sec is %d\n", time_tm.tm_sec);
	
	if(set_alarm_time(&sTime, &sDate, &sAlarm, time_bcd, &time_tm) != 0)
	{
		printf("set alarm time failed");
		return -1;
	}
	
	return 0;
	
}

int again_set_sample_time()
{
	long now_time;
	long alarm_time;
	struct tm* palarm_time_tm;
	uint8_t alarm_time_bcd[6] = {0};
	RTC_AlarmTypeDef sAlarm = {0};
	
	time_t unix_time;
	
	if(get_unix_time(&now_time) != 0)
	{
		return -1;
	}
	
	alarm_time = now_time + 15;
	unix_time = (time_t)alarm_time;
	
	palarm_time_tm = localtime(&unix_time);
	
	Tm2BcdTime(palarm_time_tm, alarm_time_bcd);
	
	printf("alarm_year is 0x%02x\n", alarm_time_bcd[4]);
	printf("alarm_month is 0x%02x\n", alarm_time_bcd[5]);
	printf("alarm_data is 0x%02x\n", alarm_time_bcd[0]);
	printf("alarm_hour is 0x%02x\n", alarm_time_bcd[1]);
	printf("alarm_min is 0x%02x\n", alarm_time_bcd[2]);
	printf("alarm_sec is 0x%02x\n", alarm_time_bcd[3]);
	
	sAlarm.AlarmTime.Hours = alarm_time_bcd[1];
	sAlarm.AlarmTime.Minutes = alarm_time_bcd[2];
	sAlarm.AlarmTime.Seconds = alarm_time_bcd[3];
	sAlarm.AlarmTime.SubSeconds = 0x0;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = alarm_time_bcd[0];
  sAlarm.Alarm = RTC_ALARM_A;
	
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
		printf("HAL_RTC_SetAlarm_IT failed");
    return -1;
  }
	
	return 0;
	
}
