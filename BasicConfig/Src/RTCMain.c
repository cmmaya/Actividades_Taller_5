#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "RTCDriver.h"


//Defino variables
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerPinRx = { 0 };

BasicTimer_Handler_t handlerStateLed = { 0 };

USART_Handler_t handlerCommTerminal = { 0 };
uint8_t rxData = 0;
char bufferData[64] = "esto es una prueba";
char greetingMsg[] = "Taller V Rocks!\n";

char *datos;

//Definicion de funciones
void initSystem(void);

//Funcion Principal
int main(void) {
	initSystem();
	rtc_Config();

	rtc_SetTime(2, 2, 2);
	while (1) {
		datos = rtc_GetData();
		if (rxData != '\0') {

			if (rxData == 'm') {
				writeMsg(&handlerCommTerminal, greetingMsg);
				rxData = '\0';
			}

			else if (rxData == 'x') {
				uint8_t X0 = datos[4];
				sprintf(bufferData, "dataX = %d \n", X0);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else {
				rxData = '\0';
			}

		}
	}
}


void initSystem(void){

	//Pin que controla el led de la board
	handlerLedOK.pGPIOx 							= GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber  	= PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed  		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerLedOK);

	//config los pines del usart
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber  	= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;
	GPIO_Config(&handlerPinTx);


	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber  	= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;
	GPIO_Config(&handlerPinRx);

	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_57600;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_9BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_EVEN;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerCommTerminal);

	//Config el Timer
	handlerStateLed.ptrTIMx 						= TIM2;
	handlerStateLed.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerStateLed.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerStateLed.TIMx_Config.TIMx_period 		= 250;	//periodod de 250 ms
	handlerStateLed.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerStateLed);
	startTimer(&handlerStateLed);

}

void usart2Rx_Callback(void){
	rxData = getRxData();
	}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLedOK);
}
