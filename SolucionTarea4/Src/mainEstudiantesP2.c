/**
 **************************************************************************************************
 * @file           : MainTaller7Parte2.c
 * @authors        : Alejandro Vélez Patrouilleau - alvelezp@unal.edu.co
 * 				   : Juan Pablo Toro Arenas		  - jutoroa@unal.edu.co
 * @brief          : Introduccion al sprintF y a la conversión ADC:
 *
 **************************************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"

char array[64];
char value = 19;
int m = 0;
int num = 78;
int digit = 0;

// Definicion de los handlers necesarios__________________________
GPIO_Handler_t handlerStateLed = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerEncoderSup = { 0 };
GPIO_Handler_t handlerEncoderInf = { 0 };
GPIO_Handler_t handlerUserButton = { 0 };

// Handlers del 7 sementos________________________________________
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
char NUM[] = { 0 };
int n = 0;

//_______________________________________________________________________________________
// Timers
BasicTimer_Handler_t handlerStateTimer = { 0 };
BasicTimer_Handler_t handlerFrecuencyTimer = { 0 };
char callback3flag = 0;
char blinkyflag = 0;

//_______________________________________________________________________________________
// Utiliza la conexion USB
USART_Handler_t handlerUsart2 = { 0 };
//_______________________________________________________________________________________
// Configuración Exti_
EXTI_Config_t handlerIntEncoder = { 0 };
EXTI_Config_t handlerIntButton  = { 0 };

//_______________________________________________________________________________________
//variables EXTI
char extiFlag = 0;
char ButtonFlag = 0;
char contador = 0;

//_______________________________________________________________________________________
// Variables USART
char ButtonMsg[] = "Cualquier mensaje \n";
char bufferData[64];

//_______________________________________________________________________________________
void InitSystem(void);
void numbers(uint8_t PinNumber);
void displayNumbers(char num);
int main(void) {


	while(num != 0){
		digit = num % 10;
		num = num / 10;
		NUM[m]=digit;
		m=m+1;
	}
	InitSystem();
	while (1) {


		displayNumbers(num);

		if (blinkyflag == 1) {
			blinkyflag = 0;
			GPIOxTooglePin(&handlerStateLed);

		}

		if (extiFlag) {
			extiFlag = 0;
			if (GPIO_ReadPin(&handlerEncoderInf)) {
				contador += 1;
				sprintf(bufferData, "CW  %u \n", contador);
				writeMsg(&handlerUsart2, bufferData);
			} else {
				contador -= 1;
				sprintf(bufferData, "CCW  %u \n", contador);
				writeMsg(&handlerUsart2, bufferData);
			}
		} else if (ButtonFlag) {
			ButtonFlag = 0;
			writeMsg(&handlerUsart2, ButtonMsg);

		}

	}
	return 0;
}

void callback_extInt2(void) {
	extiFlag = 1;
}

void callback_extInt13(void) {
	ButtonFlag = 1;
}

void BasicTimer2_Callback(void) {
	blinkyflag = 1;
}
void BasicTimer3_Callback(void) {
	callback3flag = 1;
}

void InitSystem(void) {

// Configurando el pin para el Led_Blinky
	handlerStateLed.pGPIOx = GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStateLed);

//Configuramos el Boton
	handlerUserButton.pGPIOx = GPIOB;               //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;   //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN; //tipo entrada
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP; //pull up pull down
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_MEDIUM; // vel media
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode = AF0; //Ninguna función.
	GPIO_Config(&handlerUserButton);

//_________________________________________________________________________________________
// Configurando los pines sobre los que funciona el USART2 (TX)
	handlerPinTx.pGPIOx = GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTx);

// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx = USART2;
	handlerUsart2.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerUsart2);
//_______________________________________________________________________________________
// Configurando el Timer 2 y 3
	handlerStateTimer.ptrTIMx = TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period = 250;
	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer); 	// Activamos el TIM2

	handlerFrecuencyTimer.ptrTIMx = TIM3;
	handlerFrecuencyTimer.TIMx_Config.TIMx_mode = BTIMER_MODE_UP;
	handlerFrecuencyTimer.TIMx_Config.TIMx_speed = BTIMER_SPEED_1ms;
	handlerFrecuencyTimer.TIMx_Config.TIMx_period = 250;
	BasicTimer_Config(&handlerFrecuencyTimer);
	startTimer(&handlerFrecuencyTimer); 	// Activamos el TIM3
//_______________________________________________________________________________________
			//Configuramos el PIN del encoder Correspondiente a la terminal A
	handlerEncoderSup.pGPIOx = GPIOB;
	handlerEncoderSup.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerEncoderSup.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	GPIO_Config(&handlerEncoderSup);

//Configuramos el PIN del encoder Correspondiente a la terminal B
	handlerEncoderInf.pGPIOx = GPIOC;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerEncoderInf);

//_______________________________________________________________________________________
//Configuramos los pines del 7 segmentos
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

	GPIO_WritePin(&handlerPA13, RESET);
	GPIO_WritePin(&handlerPA15, RESET);
	GPIO_WritePin(&handlerPC13, RESET);
	GPIO_WritePin(&handlerPC10, RESET);
	GPIO_WritePin(&handlerPC12, RESET);
	GPIO_WritePin(&handlerPC11, RESET);
	GPIO_WritePin(&handlerPB7, RESET);
	GPIO_WritePin(&handlerPA14, RESET);
	GPIO_WritePin(&handler3, RESET);
	GPIO_WritePin(&handler2, SET);

//Configuramos el Exti del encoder
	handlerIntEncoder.pGPIOHandler = &handlerEncoderSup;
	handlerIntEncoder.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntEncoder);

	handlerIntButton.pGPIOHandler = &handlerUserButton;
	handlerIntButton.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntButton);

}

void numbers(uint8_t PinNumber) {
	GPIO_WritePin(&handlerPA13, RESET);
	GPIO_WritePin(&handlerPA15, RESET);
	GPIO_WritePin(&handlerPC13, RESET);
	GPIO_WritePin(&handlerPC10, RESET);
	GPIO_WritePin(&handlerPC12, RESET);
	GPIO_WritePin(&handlerPC11, RESET);
	GPIO_WritePin(&handlerPB7, RESET);
	GPIO_WritePin(&handlerPA14, RESET);
	switch (PinNumber) {
	case 0: {
		GPIO_WritePin(&handlerPA14, SET);
		GPIO_WritePin(&handlerPB7, SET);

		GPIO_WritePin(&handlerPC11, SET);
		GPIO_WritePin(&handlerPC12, SET);
		GPIO_WritePin(&handlerPC12, SET);
		GPIO_WritePin(&handlerPC10, SET);
		GPIO_WritePin(&handlerPB7, SET);
		GPIO_WritePin(&handlerPC13, SET);
		break;
	}
		case 1:
		{
			GPIO_WritePin(&handlerPC13, SET);
			GPIO_WritePin(&handlerPC11, SET);
			break;
		}

		case 2:
		{
			GPIO_WritePin(&handlerPA14, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC13, SET);
			GPIO_WritePin(&handlerPC10, SET);
			GPIO_WritePin(&handlerPC12, SET);
			break;
		}
		case 3:
		{
			GPIO_WritePin(&handlerPA14, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC13, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPC12, SET);
			break;

		}
		case 4:
		{
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC13, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPA15, SET);
			break;
		}
		case 5:
		{
			GPIO_WritePin(&handlerPA14, SET);
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPC12, SET);
			GPIO_WritePin(&handlerPA15, SET);
			break;
		}
		case 6:
		{
			GPIO_WritePin(&handlerPA14, SET);
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPC12, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC10, SET);
			GPIO_WritePin(&handlerPB7, SET);
			break;
		}
		case 7:
		{
			GPIO_WritePin(&handlerPC13, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPA14, SET);
			break;
		}
		case 8:
		{
			GPIO_WritePin(&handlerPA14, SET);
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPC12, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC10, SET);
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPC13, SET);
			break;
		}
		case 9:
		{
			GPIO_WritePin(&handlerPA14, SET);
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPC11, SET);
			GPIO_WritePin(&handlerPC12, SET);
			GPIO_WritePin(&handlerPA15, SET);
			GPIO_WritePin(&handlerPB7, SET);
			GPIO_WritePin(&handlerPC13, SET);
			break;
		}

	}
	}

//	void displayNumbers(char NUM[]) {
//
//		//	    while(num != 0){
//		//	        digit = num % 10;
//		//	        num = num / 10;
//		//	        NUM[m]=digit;
//		//	        m=m+1;
//		//	    }
//
//
//		if (blinkyflag == 1) {
//			blinkyflag = 0;
//			GPIOxTooglePin(&handlerStateLed);
//
//		}
//
//		if (n < 2) {
//			if (callback3flag == 1) {
//				callback3flag = 0;
//				GPIOxTooglePin(&handler2);
//				GPIOxTooglePin(&handler3);
//				numbers(NUM[n]);
//				n = n + 1;
//			}
//		}
//
//		else {
//			n = 0;
//		}
//	}

	void displayNumbers(char num) {




		if (n < 2) {
			if (callback3flag == 1) {
				callback3flag = 0;
				GPIOxTooglePin(&handler2);
				GPIOxTooglePin(&handler3);
				numbers(NUM[n]);
				n = n + 1;
			}
		}

		else {
			n = 0;
		}
	}
