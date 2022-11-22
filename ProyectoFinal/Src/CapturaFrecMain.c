/*
 * CapturaFrecMain.c
 */


#include "stm32f4xx.h"

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"

/* Definiciones de la variables */

GPIO_Handler_t handlerPinEnable = {0};
GPIO_Handler_t handlerPinBase = {0};
uint32_t detection = 0;

GPIO_Handler_t handlerLedOk = {0};
BasicTimer_Handler_t handlerStaterOKTimer = {0};

GPIO_Handler_t handlerPinPwmChannel		= {0};
PWM_Handler_t handlerSignalPWM			= {0};
uint16_t duttyValue = 185;

GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferData[64] = "esto es una pequena prueba";

#define izquierda 	0
#define derecha 	1
#define stop 		2


/* definicion de las funciones prototipo */
void initSystem(void);
void girarServo(uint8_t dir);
void USARTCommands(void);

/**
 * Funcion principal del programa
 */
int main(void){
	// Llamamos a la funcion que nos inicializa el hardware del sistema
	initSystem();

	/* Main loop */
	while(1){
		USARTCommands();
	}
}

/*
 *
 *
 */
void initSystem(void){
	// Organizamos la configuracion del pin que controla el LED de la board
	handlerLedOk.pGPIOx								= GPIOA;
	handlerLedOk.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerLedOk.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLedOk.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;		//8
	handlerLedOk.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLedOk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLedOk.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerLedOk);

	/* Configuramos los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPinTX);

	/* configurando los pines sobre los que funciona el USART */
	handlerPinRX.pGPIOx								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPinRX);

	// Configuramos la comunicacion serial
	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_9600;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerCommTerminal);

	// Configuramos el timer
	handlerStaterOKTimer.ptrTIMx 							= TIM2;
	handlerStaterOKTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerStaterOKTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerStaterOKTimer.TIMx_Config.TIMx_period 			= 250;	//periodod de 250 ms
	handlerStaterOKTimer.TIMx_Config.TIMx_interruptEnable	= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerStaterOKTimer);
	startTimer(&handlerStaterOKTimer);

	handlerPinEnable.pGPIOx								= GPIOC;
	handlerPinEnable.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerPinEnable.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerPinEnable.GPIO_PinConfig.GPIO_PinNumber		= PIN_11;
	handlerPinEnable.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinEnable.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPinEnable);

	handlerPinBase.pGPIOx								= GPIOC;
	handlerPinBase.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerPinBase.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerPinBase.GPIO_PinConfig.GPIO_PinNumber		= PIN_12;
	handlerPinBase.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinBase.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinBase.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPinBase);

	/* Configuramos el PWM */
	handlerPinPwmChannel.pGPIOx								= GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	GPIO_Config(&handlerPinPwmChannel);

	handlerSignalPWM.config.channel 	= PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle 	= duttyValue;
	handlerSignalPWM.config.periodo 	= 200;
	handlerSignalPWM.config.prescaler 	= 1600;
	handlerSignalPWM.ptrTIMx			= TIM3;

	pwm_Config(&handlerSignalPWM);
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);
}

/* Callback relacionado con la recepcion del USART2*/

void usart2Rx_Callback(void){
	rxData = getRxData();
}

void BasicTimer2_Callback(void){
	// handlerLedOK
	GPIOxTooglePin(&handlerLedOk);
}

void girarServo(uint8_t dir){
	if(dir == izquierda){
		updateDuttyCycle(&handlerSignalPWM, 181);
		//entre 180 y 185 se mueve hacia la izquierda
	}
	else if(dir == derecha){
		updateDuttyCycle(&handlerSignalPWM, 189);
		//entre 185 y 190 se mueve hacia la derecha
	}
	else if(dir == stop){
		updateDuttyCycle(&handlerSignalPWM, 200);
		//entre 185 y 190 se mueve hacia la derecha
	}
}

void USARTCommands(void){
	//Hacemos un eco con el valor que nos llega por el serial
	if(rxData != '\0'){
		if(rxData == 'm'){
			writeMsg(&handlerCommTerminal, "TallerVRocks! \n");
			rxData = '\0';
		}

		//Probamos el Detector IR
		else if(rxData == 's'){
			detection = GPIO_ReadPin(&handlerPinEnable);
			sprintf(bufferData, "rawPeriod = %u \n", (unsigned int) detection);
			writeMsg(&handlerCommTerminal, bufferData);
		}

		//Probamos el motor DC
		else if(rxData == 'h'){
			GPIO_WritePin(&handlerPinBase, SET);
			writeMsg(&handlerCommTerminal, "on \n");

			rxData = '\0';
		}
		else if(rxData == 'l'){
			GPIO_WritePin(&handlerPinBase, RESET);
			writeMsg(&handlerCommTerminal, "off \n");
			rxData = '\0';
		}


		//Probar Servo
		else if(rxData == 'd'){
			writeMsg(&handlerCommTerminal, "derecha \n");
			girarServo(derecha);
			rxData = '\0';
		}
		else if(rxData == 'i'){
			writeMsg(&handlerCommTerminal, "izquierda \n");
			girarServo(izquierda);
			//entre 180 y 185 se mueve hacia la izquierda
			rxData = '\0';
		}
		else if(rxData == 'p'){
			writeMsg(&handlerCommTerminal, "pausa \n");
			girarServo(stop);
			//entre 180 y 185 se mueve hacia la izquierda
			rxData = '\0';
		}
		rxData = '\0';

	}
}
