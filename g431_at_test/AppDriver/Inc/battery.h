#ifndef __BATTERY_H

#define __BATTERY_H

#include "stm32g4xx_hal.h"

#include "adc.h"
#define COLLECT_NUM 5

extern float adcGetElectricQuantity(void);



#endif /*__BATTERY_H*/


