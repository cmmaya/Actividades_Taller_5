/*
 * main2.c
 *
 *  Created on: Oct 13, 2022
 *      Author: Camilo
 */
#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"

GPIO_Handler_t handlerPC13 = { 0 };
GPIO_Handler_t handlerPC11 = { 0 };
GPIO_Handler_t handlerPA15 = { 0 };
GPIO_Handler_t handlerPA13 = { 0 };
GPIO_Handler_t handlerPC10 = { 0 };
GPIO_Handler_t handlerPC12 = { 0 };
GPIO_Handler_t handlerPB7 = { 0 };
GPIO_Handler_t handlerPA14 = { 0 };
GPIO_Handler_t handler2 = { 0 };
GPIO_Handler_t handler3 = { 0 };
GPIO_Handler_t handlerStateLed = { 0 };

uint8_t PIN = 9;


BasicTimer_Handler_t handlerStateTimer     = { 0 };
BasicTimer_Handler_t handlerFrecuencyTimer = { 0 };

//void configurarGPIOleds(GPIO_TypeDef *GPIOX  ,uint8_t PIN_X, GPIO_Handler_t *handlerPXX){
//	handlerPXX->pGPIOx =  GPIOX;
//	handlerPXX->GPIO_PinConfig.GPIO_PinNumber = PIN_X;
//	handlerPXX->GPIO_PinConfig.GPIO_PinMode   = GPIO_MODE_OUT;
//	handlerPXX->GPIO_PinConfig.GPIO_PinSpeed  = GPIO_OSPEED_FAST;
//}

void numbers(uint8_t PinNumber){

	switch(PinNumber){
	case 1:{
		GPIO_WritePin(&handlerPC13,SET);
		GPIO_WritePin(&handlerPC11,SET);
		break;
	}

	case 2:{
		GPIO_WritePin(&handlerPA14,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC13,SET);
		GPIO_WritePin(&handlerPC10,SET);
		GPIO_WritePin(&handlerPC12,SET);
		break;
	}
	case 3:{
		GPIO_WritePin(&handlerPA14,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC13,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPC12,SET);
		break;

	}
	case 4:{
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC13,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPA15,SET);
		break;
	}
	case 5:{
		GPIO_WritePin(&handlerPA14,SET);
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPC12,SET);
		GPIO_WritePin(&handlerPA15,SET);
		break;
	}
	case 6:{
		GPIO_WritePin(&handlerPA14,SET);
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPC12,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC10,SET);
		GPIO_WritePin(&handlerPB7,SET);
		break;
	}
	case 7:{
		GPIO_WritePin(&handlerPC13,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPA14,SET);
		break;
	}
	case 8:{
		GPIO_WritePin(&handlerPA14,SET);
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPC12,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC10,SET);
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPC13,SET);
		break;
	}
	case 9:{
		GPIO_WritePin(&handlerPA14,SET);
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPC11,SET);
		GPIO_WritePin(&handlerPC12,SET);
		GPIO_WritePin(&handlerPA15,SET);
		GPIO_WritePin(&handlerPB7,SET);
		GPIO_WritePin(&handlerPC13,SET);
		break;
	}


	}
}

int main(void){
//	 Configurando el pin para el Led_Blinky
//	configurarGPIOleds(GPIOC,PIN_13,&handlerPC13);
//	configurarGPIOleds(GPIOC,PIN_11,&handlerPC11);
//	configurarGPIOleds(GPIOA,PIN_15,&handlerPA15);
//	configurarGPIOleds(GPIOA,PIN_13,&handlerPA13);
	// Configurando el Timer 2 y 3
	handlerStateTimer.ptrTIMx = TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode   = BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed  = BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period = 250;
	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer); 	// Activamos el TIM2

	handlerStateLed.pGPIOx = GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStateLed);

	handlerPC13.pGPIOx = GPIOC;
	handlerPC13.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerPC13.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPC13.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPC13);

	handlerPC11.pGPIOx = GPIOC;
	handlerPC11.GPIO_PinConfig.GPIO_PinNumber = PIN_11;
	handlerPC11.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPC11.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPC11);

	handlerPA15.pGPIOx = GPIOA;
	handlerPA15.GPIO_PinConfig.GPIO_PinNumber = PIN_15;
	handlerPA15.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPA15.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPA15);

	handlerPA13.pGPIOx = GPIOA;
	handlerPA13.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerPA13.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPA13.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPA13);

	handlerPC10.pGPIOx = GPIOC;
	handlerPC10.GPIO_PinConfig.GPIO_PinNumber = PIN_10;
	handlerPC10.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPC10.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPC10);

	handlerPC12.pGPIOx = GPIOC;
	handlerPC12.GPIO_PinConfig.GPIO_PinNumber = PIN_12;
	handlerPC12.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPC12.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPC12);

	handlerPB7.pGPIOx = GPIOB;
	handlerPB7.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
	handlerPB7.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPB7.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPB7);

	handlerPA14.pGPIOx = GPIOA;
	handlerPA14.GPIO_PinConfig.GPIO_PinNumber = PIN_14;
	handlerPA14.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerPA14.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPA14);


	handler2.pGPIOx = GPIOC;
	handler2.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
	handler2.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handler2.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handler2);

	handler3.pGPIOx = GPIOC;
	handler3.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handler3.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handler3.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	GPIO_Config(&handler3);



	GPIO_WritePin(&handlerPA13,RESET);
	GPIO_WritePin(&handlerPA15,RESET);
	GPIO_WritePin(&handlerPC13,RESET);
	GPIO_WritePin(&handlerPC10,RESET);
	GPIO_WritePin(&handlerPC12,RESET);
	GPIO_WritePin(&handlerPC11,RESET);
	GPIO_WritePin(&handlerPB7,RESET);
	GPIO_WritePin(&handlerPA14,RESET);

	while(1){
		numbers(PIN);



		GPIO_WritePin(&handler2,RESET);
		GPIO_WritePin(&handler3,RESET);
	}
}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerStateLed);

}
