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
GPIO_Handler_t handlerLedOk = {0};
GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};

BasicTimer_Handler_t handlerStaterOKTimer = {0};

GPIO_Handler_t handlerPinPwmChannel		= {0};
PWM_Handler_t handlerSignalPWM			= {0};

GPIO_Handler_t handlerPinCaptureFrec	= {0};
uint32_t rawPeriod = 0;

USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferData[64] = "esto es una pequena prueba";

Capture_Handler_t handlerCapturaFrec = {0};
char flagCapture = 0;
char flagStart = 1;
uint8_t duttyValue = 5;
/* definicion de las funciones prototipo */
void initSystem(void);

/**
 * Funcion principal del programa
 */
int main(void){
	// Llamamos a la funcion que nos inicializa el hardware del sistema
	initSystem();
	writeMsg(&handlerCommTerminal, "Bienvenido, porfavor inicializar: 's' \n -para adquirir el dato presionar 't' \n");

	/* Main loop */
	while(1){
		/*Hacemos un eco con el valor que nos llega por el serial*/
		if(rxData != '\0'){

			/*inicializamos el capturador de frecuencia*/
			if(rxData == 's'){
				//Le decimos que ejecute el start solo 1 vez por sesion
				if(flagStart){
				//llamamos la funcion start
				startCapture(&handlerCapturaFrec);
				//escribimos start el el USART 2
				writeMsg(&handlerCommTerminal, "start \n");
				}
				flagStart = 0;
				rxData = '\0';
			}
			/*leemos el valor del rawperiod*/
			else if(rxData == 't'){
				//escribimos el valor del periodo en el USART 2
				sprintf(bufferData, "rawPeriod = %u \n", (unsigned int) rawPeriod);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			/*para probar la comunicacion serial*/
			else if(rxData == 'm'){
				writeMsg(&handlerCommTerminal, "TallerVRocks! \n");
				rxData = '\0';
			}

			rxData = '\0';
		}
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


	// Llamamos a la funcion config, para que se encargue de cargar la configuracion
	// en los registros adecuados
	GPIO_Config(&handlerLedOk);

	// Llevamos al LED a un estado de encendido (ON o SET)
	GPIO_WritePin(&handlerLedOk, SET);

	/* Configuramos los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	// Llamamos a la funcion config, para que se encargue de cargar la configuracion
	// en los registros adecuados
	GPIO_Config(&handlerPinTX);

	/* configurando los pines sobre los que funciona el USART */
	handlerPinRX.pGPIOx								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	// Llamamos a la funcion config, para que se encargue de cargar la configuracion
	// en los registros adecuados
	GPIO_Config(&handlerPinRX);


	// Configuramos la comunicacion serial
	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_115200;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;


	// Llamamos a la funcion config, para que se encargue de cargar la configuracion
	// en los registros adecuados
	USART_Config(&handlerCommTerminal);

	// Configuramos el timer
	handlerStaterOKTimer.ptrTIMx 							= TIM2;
	handlerStaterOKTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerStaterOKTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerStaterOKTimer.TIMx_Config.TIMx_period 			= 250;	//periodod de 250 ms
	handlerStaterOKTimer.TIMx_Config.TIMx_interruptEnable	= BTIMER_INTERRUPT_ENABLE;

	// Cargamos la configuracion
	BasicTimer_Config(&handlerStaterOKTimer);
	startTimer(&handlerStaterOKTimer);

	/* Llamamos la funcion para configurar y activar el SysTick */
	// config_SysTickMs();

	/* Configuramos el PWM */
	handlerPinPwmChannel.pGPIOx								= GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	//cargamos la config del pin del PWM como ALTFN 2, correspondiente al canal 2 del tim 3
	GPIO_Config(&handlerPinPwmChannel);

	handlerSignalPWM.config.channel 	= PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle 	= duttyValue;
	handlerSignalPWM.config.prescaler 	= CAPTURE_TIMER_SPEED_1ms; //se hace un conteo cada 16000 ciclos (1ms)
	handlerSignalPWM.config.periodo 	= 150;  //el periodo es de 20ms
	handlerSignalPWM.ptrTIMx			= TIM3;

	pwm_Config(&handlerSignalPWM);
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);

	// Timer 4...
	/* Configuramos el pin para la captura de la senal,
	 * que sera el pin PB8, canal CH3 del Timer 4
	 */
	handlerPinCaptureFrec.pGPIOx								= GPIOB;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinCaptureFrec.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	//cargamos la config con ALFN 2 correspondiente al canal  del TIM4
	GPIO_Config(&handlerPinCaptureFrec);

	//configuramos el capturador de frecuencia
	handlerCapturaFrec.ptrTIMx					= TIM4;
	handlerCapturaFrec.config.channel			= CAPTURE_CHANNEL_3;
	handlerCapturaFrec.config.edgeSignal   		= CAPTURE_FALLING_EDGE; /*se hace la captura en flanco de bajada*/
	handlerCapturaFrec.config.prescalerCapture	= CAPTURE_PREESCALER_1_1;
	handlerCapturaFrec.config.timerSpeed		= CAPTURE_TIMER_SPEED_1ms;
	handlerCapturaFrec.config.enableInt			= INTERRUPT_ENABLE;     /*Activamos las interrupciones*/
	//cargamos la configuracion
	capture_Config(&handlerCapturaFrec);
}

/* Callback relacionado con la recepcion del USART2
 * Debo leer el puerto para bajar la bandera de la interupcion
 */
void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega
	// Esto ademas debe bajar la bandera de la interrupcion
	rxData = getRxData();
}

/**/
void BasicTimer2_Callback(void){
	// Hacemos un blinky para indicar que el equipoesta funcinando correctamente.
	// handlerLedOK
	GPIOxTooglePin(&handlerLedOk);
}

/*Callback de la captura del Timer 4, canal 3*/
void TimerCapture4_Callback(void){
	/*vamos a hacer 2 capturas:
	 *la primera es cuando la bandera está en 0. En ese caso tomamos el valor del counter desde que empezó en un
	  momento indeterminado
	 *La segunda es cuando la bandera está en 1. Aqui tomamos el dato de nuevo y los restamos con el anterior
	  para obtener el periodo entre las 2 interrupciones
	 */
	if(flagCapture){
		//obtenemos el dato la segunda vez y restamos los 2 datos
		rawPeriod = getData()- rawPeriod;
		//Detenemos la captura y timer para que no se sigan dando interrupciones
		stopCapture(&handlerCapturaFrec);
	}

	else{
		//obtenemos el dato la primera vez y subimos la bandera
		rawPeriod = getData() ;
		flagCapture = 1;

	}
}
