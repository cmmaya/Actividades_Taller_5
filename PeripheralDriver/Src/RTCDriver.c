/*
 * RTCDriver.c
 */


#include <stdint.h>
#include "RTCDriver.h"

void rtc_Config(void){
	//1.Desbloqueamos el PWR para poder modificar los registros

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;

	RCC->BDCR |= RCC_BDCR_LSEON;
	RCC->BDCR |= RCC_BDCR_RTCEN;
	RCC->BDCR |= RCC_BDCR_RTCSEL_0;

	RTC->WPR =0xCA;
	RTC->WPR =0x53;


	//2. Calendario
	//Lo inicializamos (se reinicia los valores)
	rtc_Initialize();
	RTC->PRER |= 255;
	RTC->PRER |= 127 << RTC_PRER_PREDIV_A_Pos;
	RTC->CR   |= RTC_CR_COSEL;

	RTC->CR  |=RTC_CR_BYPSHAD; //taken directly from the calendary counter

	rtc_SetTime(0,0,0);
	rtc_SetDate(0,0);

	//funciona con formato 24h
	RTC->CR   &= ~RTC_CR_FMT;
	RTC->TR   &= ~RTC_TR_PM;

	rtc_NotInitialize();


}

void	rtc_Initialize(void){
	RTC->ISR |= RTC_ISR_INIT;
	while(!(RTC->ISR & RTC_ISR_INIT)){
		__NOP();
	}
}

void rtc_NotInitialize(void){
	RTC->ISR &= ~RTC_ISR_INIT;

}

void	rtc_SetTime(uint8_t hours,uint8_t minutes, uint8_t seconds){
	rtc_Config();
	rtc_Initialize();
	uint8_t horadec = hours / 10;
	uint8_t horaunid = hours % 10;
	uint8_t minutodec = minutes / 10;
	uint8_t minutounid = minutes % 10;
	uint8_t segundodec = seconds / 10;
	uint8_t segundounid = seconds % 10;
	RTC->TR &= ~RTC_TR_HT;
	RTC->TR &= ~RTC_TR_HU;
	RTC->TR &= ~RTC_TR_MNT;
	RTC->TR &= ~RTC_TR_MNU;
	RTC->TR &= ~RTC_TR_ST;
	RTC->TR &= ~RTC_TR_SU;

	RTC->TR |= horadec << (RTC_TR_HT_Pos);
	RTC->TR |= horaunid << (RTC_TR_HU_Pos);

	RTC->TR |= minutodec << (RTC_TR_MNT_Pos);
	RTC->TR |= minutounid << (RTC_TR_MNU_Pos);

	RTC->TR |= segundodec << (RTC_TR_ST_Pos);
	RTC->TR |= segundounid << (RTC_TR_SU_Pos);

	rtc_NotInitialize();

}

void	rtc_SetDate(uint8_t year,uint8_t month){
	uint8_t yeardec = year / 10;
	uint8_t yearunid = year % 10;
	uint8_t monthdec = month / 10;
	uint8_t monthunid = month % 10;

	RTC->DR &= ~RTC_DR_YT;
	RTC->DR &= ~RTC_DR_YU;
	RTC->DR &= ~RTC_DR_MT;
	RTC->DR &= ~RTC_DR_MU;

	RTC->DR |= yeardec << (RTC_DR_YT_Pos);
	RTC->DR |= yearunid << (RTC_DR_YU_Pos);

	RTC->DR |= monthdec << (RTC_DR_MT_Pos);
	RTC->DR |= monthunid << (RTC_DR_MU_Pos);
}




char * rtc_GetData(void){
	static char array[10];
	//segundo
	array[0]= (RTC->TR & RTC_TR_SU)>>RTC_TR_SU_Pos;
	array[1]= (RTC->TR & RTC_TR_ST)>>RTC_TR_ST_Pos;
	//minutos
	array[2]= (RTC->TR & RTC_TR_MNU)>>RTC_TR_MNU_Pos;
	array[3]= (RTC->TR & RTC_TR_MNT)>>RTC_TR_MNT_Pos;
	//horas
	array[4]= (RTC->TR & RTC_TR_HU)>>RTC_TR_HU_Pos;
	array[5]= (RTC->TR & RTC_TR_HT)>>RTC_TR_HT_Pos;
	//month
	array[6]= (RTC->DR & RTC_DR_MU)>>RTC_DR_MU_Pos;
	array[7]= (RTC->DR & RTC_DR_MT)>>RTC_DR_MT_Pos;
	//anhos
	array[8]= (RTC->DR & RTC_DR_YU)>>RTC_DR_YU_Pos;
	array[9]= (RTC->DR & RTC_DR_YT)>>RTC_DR_YT_Pos;

	return array;
}

