/*
 * RTCDriver.h
 *
 *  Created on: Nov 1, 2022
 *      Author: Camilo
 */

#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_

#include "stm32f411xe.h"

typedef struct{
	uint8_t 		hours;
	uint8_t			minutes;
	uint8_t 		seconds;
} RTC_Handler_t;

void 	rtc_Config(void);
void	rtc_Initialize(void);
void	rtc_SetTime(uint8_t hours,uint8_t minutes, uint8_t seconds);
void	rtc_SetDate(uint8_t year,uint8_t month);
void 	rtc_NotInitialize(void);
char * rtc_GetData(void);

#endif /* RTCDRIVER_H_ */
