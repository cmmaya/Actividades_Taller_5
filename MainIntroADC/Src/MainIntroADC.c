/*
 * MainIntroADC.c

 */


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"
#include "ExtiDriver.h"

// Definicion de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin 	= { 0 };
GPIO_Handler_t handlerUserButton 	= { 0 };
GPIO_Handler_t handlerTxPin 		= { 0 };
GPIO_Handler_t handlerRxPin			= { 0 };
EXTI_Config_t handlerPinTriggerAdc  = { 0 };
GPIO_Handler_t GPIOHandlerEXTI			= { 0 };


// Timer encargado del Blinky
BasicTimer_Handler_t hanlderBlinkyTimer = { 0 };

// COmunicacion Rs_232 con el PC, ya habilitada en la board nucleo
// Utiliza la conecci[on USB
USART_Handler_t handlerUsart2 = {0};
uint8_t rxData = {0};
char bufferData[64] = {0};

// Elementos necesarios para configurar el ADC
ADC_Config_t adcConfig = {0};
uint16_t adcData;
bool adcIsComplete = false;

// Mensaje que se imprime
char greetingMsg[] = "Taller V Rocks!\n";

// Definicion de las cabeceras de las funciones del main
void initSystem(void);
	/**
	 * Funcion principal del programa
	 * Esta funcion es el corazon del programa
	 */
int main(void){
	// inicializamos todos los elementos del sistema
	initSystem();
	writeChar(&handlerUsart2, ' ');

	/* loop forever */
	while(1){
		// EL sistema siempre esta verificando si el valor del Rx data ha cambiado
		// lo cual suscede en la ISR de la recepcion (RX)
		// Si este valor deja de ser "\0" significa que recibio un caracter, por lo tanto
		// entra en el bloque if para analizar que se recibio
		if(rxData != '\0'){
			writeChar(&handlerUsart2, rxData);
			if(rxData == 'm'){
				writeMsg(&handlerUsart2, greetingMsg);
			}
			if(rxData == 's'){
				startSingleADC();
			}
			rxData = '\0';
		}

		// Esta otra bandera corresponde a cuando se dio una nuva conversion ADC
		// Entonces el sistema entra a ese bloque if para dar  un formato al dato
		// y enviarlo por la comunicacion serial
		if(adcIsComplete == true){
			sprintf(bufferData,"ADC = %u \n\r", adcData);
			writeMsg(&handlerUsart2, bufferData);
			adcIsComplete = false;
		}
	}
	return 0;

}

void initSystem(void){

	// Configuramos el pin para el led blinky
	handlerBlinkyPin.pGPIOx								= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerBlinkyPin);

	// Configurando el pin para el boton azul
	handlerUserButton.pGPIOx								= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerUserButton);

	// Configurando los pines sobre los que funciona el Usart2(Tx)
	handlerTxPin.pGPIOx									= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	GPIO_Config(&handlerTxPin);

	// Configurando los pines sobre los que funciona el Usart2(Rx)
	handlerRxPin.pGPIOx									= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;
	GPIO_Config(&handlerRxPin);

	//Configurando el timer 2 para que funcione con el Blinky
	hanlderBlinkyTimer.ptrTIMx 							= TIM2;
	hanlderBlinkyTimer.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	hanlderBlinkyTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	hanlderBlinkyTimer.TIMx_Config.TIMx_period 			= 250;	//periodod de 250 ms
	hanlderBlinkyTimer.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&hanlderBlinkyTimer);
	startTimer(&hanlderBlinkyTimer);

	GPIOHandlerEXTI.pGPIOx  					   	 = GPIOC;
	GPIOHandlerEXTI.GPIO_PinConfig.GPIO_PinNumber 	 = PIN_11;
	GPIOHandlerEXTI.GPIO_PinConfig.GPIO_PinMode  	 = GPIO_MODE_IN;
	GPIOHandlerEXTI.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;

	//Configuramos el PIN 11 para amzar el evento
	handlerPinTriggerAdc.pGPIOHandler    =  &GPIOHandlerEXTI;
	handlerPinTriggerAdc.edgeType  		 =  EXTERNAL_INTERRUPT_RISING_EDGE;


	// Configurando al comunicacion serial (blanco Rx, verde Tx)
	handlerUsart2.ptrUSARTx = USART2;
	handlerUsart2.USART_Config.USART_baudrate 		= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits 		= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerUsart2);

	// Cargando la configuracion para la conversion ADC
	adcConfig.channel			= ADC_CHANNEL_0;
	adcConfig.dataAlignment		= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution		= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod	= ADC_SAMPLING_PERIOD_28_CYCLES;

	// configuramos al micro, la interrupcion esta activa por defecto
	adc_Config(&adcConfig);
	configExternalTrigger(&handlerPinTriggerAdc);
	adcConfigExternal();
}


/*
 * Callback del timer 2 - Hacemos un blinky
 */
void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerBlinkyPin);
}

/*
 * Callback relacionado con la recepcion Usart2
 * el puerto es leidom en la ISR
 * para bajar la bandera de la interrupcion
 * el caracter que se lee es devuelto por la funcion getRxData
 */

void usart2Rx_Callback(void){
	rxData = getRxData();
}
// Esta funcion se ejecuta luego de una funcion adc
// es llamad por la ISR de la conversion ADC

void adcComplete_Callback(void){
	adcData = getADC();
	adcIsComplete = true;
}












