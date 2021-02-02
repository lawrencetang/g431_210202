
#include "battery.h"
#include "cmsis_os.h"


float adcGetElectricQuantity(void)
{
	float battery_level;
	//ADC 开启
	HAL_ADC_Start_IT(&hadc1);
	tos_task_delay(1);
	// 开启ADC,获得电池电量
	battery_level = HAL_ADC_GetValue(&hadc1) * 3.3 / 4095.0;
	HAL_ADC_Stop_IT(&hadc1);
	return battery_level;
}

