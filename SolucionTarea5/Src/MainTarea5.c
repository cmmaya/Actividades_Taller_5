/*
 * MCO100Mhz.c
 */



#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "InternalFrecuency.h"
#include "USARTxDriver.h"

GPIO_Handler_t handlerBlinky = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerPinRx = { 0 };
GPIO_Handler_t handlerMCO1 = { 0 };

BasicTimer_Handler_t handlerStateLed = { 0 };

USART_Handler_t handlerCommTerminal = { 0 };
uint8_t rxData = 0;
char greetingMsg[] = "Taller V Rocks!\n";
char bufferData[64] = "esto es una prueba";


void initSystem(void);

int main(void){
	initSystem();
//	setFrecuency_Mhz(100,&handlerCommTerminal);
	internalFrecuency_config(100);


	while(1){
		if (rxData != '\0') {

			if (rxData == 'm') {
				writeMsg(&handlerCommTerminal, greetingMsg);
				rxData = '\0';
			}
			else {
				rxData = '\0';
			}

		}

	}
}

void initSystem(void){
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
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_115200; //La nueva mantiza se calculo en el driver.
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerCommTerminal);

    handlerBlinky.pGPIOx                                = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber         = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed	        = GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
    GPIO_Config(&handlerBlinky);

    handlerMCO1.pGPIOx                                = GPIOA;
    handlerMCO1.GPIO_PinConfig.GPIO_PinNumber         = PIN_8;
    handlerMCO1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
    handlerMCO1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
    handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed	      = GPIO_OSPEED_FAST;
    handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;
    handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	  = AF0;
    GPIO_Config(&handlerMCO1);

	handlerStateLed.ptrTIMx 						= TIM2;
	handlerStateLed.TIMx_Config.TIMx_mode 			 = BTIMER_MODE_UP;
	handlerStateLed.TIMx_Config.TIMx_speed 			 = 10000; //Usamos un nuevo macro que coicida con 25Mhz
	handlerStateLed.TIMx_Config.TIMx_period 		 = 2500;	//periodod de 250 ms
	handlerStateLed.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerStateLed);
	startTimer(&handlerStateLed);


}


void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}

void usart2Rx_Callback(void){
	rxData = getRxData();
	}


