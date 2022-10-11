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

// Definicion de los handlers necesarios
GPIO_Handler_t handlerStateLed = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerEncoderSup = { 0 };
GPIO_Handler_t handlerEncoderInf = { 0 };
GPIO_Handler_t handlerUserButton = { 0 };
// Timer encargado del Blinky
BasicTimer_Handler_t handlerStateTimer = { 0 };

// Utiliza la conexion USB
USART_Handler_t handlerUsart2 = { 0 };

// Configuración Exti
EXTI_Config_t handlerIntEncoder = { 0 };
EXTI_Config_t handlerIntButton = { 0 };

//variables EXTI
char extiFlag   = 0;
char ButtonFlag = 0;
char contador   = 0;


// Variables USART
char ButtonMsg[] = "Cualquier mensaje \n";
char bufferData[64];

void InitSystem(void);

int main(void) {
	InitSystem();


	while(1){
		if (extiFlag) {
			extiFlag = 0;
			if (GPIO_ReadPin(&handlerEncoderInf)) {
				contador += 1;
				sprintf(bufferData, "CW  %u \n",contador);
				writeMsg(&handlerUsart2, bufferData);
			}
			else {
				contador -= 1;
				sprintf(bufferData, "CCW  %u \n",contador);
				writeMsg(&handlerUsart2, bufferData);
			}
		}
		else if(ButtonFlag){
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
	handlerUserButton.pGPIOx                                = GPIOB;               //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;              //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;        //tipo entrada
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;  //pull up pull down
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				   //Ninguna función.
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
	handlerUsart2.USART_Config.USART_parity   = USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode     = USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerUsart2);
//_______________________________________________________________________________________
	// Configurando el Timer2 para que funcione con el blinky
	handlerStateTimer.ptrTIMx = TIM2;
	handlerStateTimer.TIMx_Config.TIMx_mode   = BTIMER_MODE_UP;
	handlerStateTimer.TIMx_Config.TIMx_speed  = BTIMER_SPEED_1ms;
	handlerStateTimer.TIMx_Config.TIMx_period = 250;
	BasicTimer_Config(&handlerStateTimer);
	startTimer(&handlerStateTimer); 	// Activamos el TIM2
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
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinSpeed   = GPIO_OSPEED_FAST;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerEncoderInf);


	//Configuramos el Exti del encoder
	handlerIntEncoder.pGPIOHandler = &handlerEncoderSup;
	handlerIntEncoder.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntEncoder);

	handlerIntButton.pGPIOHandler = &handlerUserButton;
	handlerIntButton.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntButton);


}



