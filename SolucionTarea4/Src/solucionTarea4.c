/**
 **************************************************************************************************
 CAMILO MONSALVE MAYA TALLER#4

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

//Variables a utilizar
int m = 0;
char num;
int digit = 0;
char NUM[2] = { 0, 0 };
int n = 0;

// Definicion de los handlers necesarios
GPIO_Handler_t handlerStateLed = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerEncoderSup = { 0 };
GPIO_Handler_t handlerEncoderInf = { 0 };
GPIO_Handler_t handlerUserButton = { 0 };

// Handlers del 7 sementos

GPIO_Handler_t handlerPB9 = { 0 };
GPIO_Handler_t handlerPB0 = { 0 };
GPIO_Handler_t handlerPA6 = { 0 };
GPIO_Handler_t handlerPC1 = { 0 };
GPIO_Handler_t handlerPC10 = { 0 };
GPIO_Handler_t handlerPC0 = { 0 };
GPIO_Handler_t handlerPA7 = { 0 };
GPIO_Handler_t handlerPB8 = { 0 };
GPIO_Handler_t handler2 = { 0 };
GPIO_Handler_t handler3 = { 0 };

//_______________________________________________________________________________________
// Timers
BasicTimer_Handler_t handlerStateTimer = { 0 };
BasicTimer_Handler_t handlerFrecuencyTimer = { 0 };

//Variables Timer
char callback3flag = 0;
char blinkyflag = 0;

//_______________________________________________________________________________________
// Utiliza la conexion USB
USART_Handler_t handlerUsart2 = { 0 };

// Variables USART
char ButtonMsg[] = "Cualquier mensaje \n";
char bufferData[64];

//_______________________________________________________________________________________
// Configuración Exti_
EXTI_Config_t handlerIntEncoder = { 0 };
EXTI_Config_t handlerIntButton = { 0 };

//variables EXTI
char extiFlag = 0;
char ButtonFlag = 0;
char contador = 5;

//_______________________________________________________________________________________
//Cabeceras de funciones a utilizar. En el apartado inferior se xplican
void InitSystem(void);
void numbers(uint8_t PinNumber);
void GPIO_PinFastConfig(GPIO_Handler_t *pPinHandler, uint8_t PINX,
		GPIO_TypeDef *pGPIOx);
void displayNumbers(char num);

int main(void) {
	InitSystem();      //importamos las configuraciones de los pines
//_______________________________________________________________________________
	while (1) {

		displayNumbers(contador); //Ponemos a desplegar el numero por defecto en el contador

		if (blinkyflag) {
			blinkyflag = 0;  				  //Bajamos la bandera
			GPIOxTooglePin(&handlerStateLed); //Activamos el blinky

		}

//__________________________________________________________________________________
		//Cuando la bandera se levanta es porque la señal de handlerEncoderSup es 0 (Falling edge)
		if (extiFlag) {

			extiFlag = 0;									//Bajamos la bandera

			if (GPIO_ReadPin(&handlerEncoderInf)) {
				contador += 1;//Usando la imagen de correo, cuando la señal superior esta en 0 y la inferior en 1, el sentido es CW
				if (contador > 50) {  		//no deja pasar al contador de 50
					contador = 0;
				}
				sprintf(bufferData, "CW  %u \n", contador);	//Transformamos el numero en un string
				writeMsg(&handlerUsart2, bufferData);		//enviamos el string

			} else {
				contador -= 1;
				if (contador > 50) {  		//no deja pasar al contador de 50
					contador = 0;
				}
				sprintf(bufferData, "CCW  %u \n", contador);
				writeMsg(&handlerUsart2, bufferData);	//mandamos el mensaje

			}

		} else if (ButtonFlag) { //cuando se acciona el boton, se detiene la señal y manda una bandera
			ButtonFlag = 0;									//Luego la bajamos
			writeMsg(&handlerUsart2, ButtonMsg);		//Y enviamos un mensaje

		}

	}

	return 0;
}

//callback de la interrupcion generada por el PIN asociado al boton
void callback_extInt0(void) {
	extiFlag = 1;
}

//callback de la interrupcion generada por el PIN asociado al encoder
void callback_extInt8(void) {
	ButtonFlag = 1;
}

//callback de la interrupcion generada por el timer 2, para el blinky
void BasicTimer2_Callback(void) {
	blinkyflag = 1;
}

//callback de la interrupcion generada por el timer 3, para la pantalla
void BasicTimer3_Callback(void) {
	callback3flag = 1;
}

/*Aqui preparamos la configuracion de todos los pines del micro*/
void InitSystem(void) {

// Configurando el pin para el Led_Blinky
	handlerStateLed.pGPIOx = GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerStateLed);
	// Configurando el pin para el Boton
	handlerUserButton.pGPIOx = GPIOA;                         //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_8;    //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN; //tipo entrada
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP; //pull down
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_MEDIUM; // vel media
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;//Ninguna función.

//_________________________________________________________________________________________
//USART
// Configurando los pines sobre los que funciona el USART2 (TX)
	handlerPinTx.pGPIOx = GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode = AF7;
	GPIO_Config(&handlerPinTx);

// Configurando la comunicación serial
	handlerUsart2.ptrUSARTx = USART2;
	handlerUsart2.USART_Config.USART_baudrate = USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize = USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity = USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits = USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode = USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_enableIntRX = USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerUsart2);
//_______________________________________________________________________________________
//TIMERS
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
	handlerFrecuencyTimer.TIMx_Config.TIMx_period = 10;
	BasicTimer_Config(&handlerFrecuencyTimer);
	startTimer(&handlerFrecuencyTimer); 	// Activamos el TIM3
//_______________________________________________________________________________________
//EXTI

//Configuramos el PIN del encoder Correspondiente a la terminal A
	handlerEncoderSup.pGPIOx = GPIOA;
	handlerEncoderSup.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
	handlerEncoderSup.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	GPIO_Config(&handlerEncoderSup);

//Configuramos el PIN del encoder Correspondiente a la terminal B
	handlerEncoderInf.pGPIOx = GPIOA;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_PULLUP;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	handlerEncoderInf.GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	GPIO_Config(&handlerEncoderInf);

//Configuramos el Exti del encoder
	handlerIntEncoder.pGPIOHandler = &handlerEncoderSup;
	handlerIntEncoder.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntEncoder);

	handlerIntButton.pGPIOHandler = &handlerUserButton;
	handlerIntButton.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntButton);

//_______________________________________________________________________________________
//Configuramos los pines del 7 segmentos

//Con la funcion FasrConfig les asignamos las propiedades GPIO

	GPIO_PinFastConfig(&handlerPB8, PIN_8, GPIOB);
	GPIO_Config(&handlerPB8);
	GPIO_PinFastConfig(&handlerPB9, PIN_9, GPIOB);
	GPIO_Config(&handlerPB9);
	GPIO_PinFastConfig(&handlerPA7, PIN_7, GPIOA);
	GPIO_Config(&handlerPA7);
	GPIO_PinFastConfig(&handlerPA6, PIN_6, GPIOA);
	GPIO_Config(&handlerPA6);
	GPIO_PinFastConfig(&handlerPC0, PIN_0, GPIOC);
	GPIO_Config(&handlerPC0);
	GPIO_PinFastConfig(&handlerPC1, PIN_1, GPIOC);
	GPIO_Config(&handlerPC1);
	GPIO_PinFastConfig(&handlerPB0, PIN_0, GPIOB);
	GPIO_Config(&handlerPB0);
	GPIO_PinFastConfig(&handlerPC10, PIN_10, GPIOC);
	GPIO_Config(&handlerPC10);
	GPIO_PinFastConfig(&handler2, PIN_4, GPIOA);
	GPIO_Config(&handler2);    //controla el transistor 1
	GPIO_PinFastConfig(&handler3, PIN_6, GPIOB);
	GPIO_Config(&handler3);    //controla el transistor 2

	//Ponemos los transistores en estados contrarios
	GPIO_WritePin(&handler3, RESET);
	GPIO_WritePin(&handler2, SET);

}

/*Esta funcion le indica a la pantalla que pines prender cuando se introduce su numero correspondiente en uint8*/
void numbers(uint8_t PinNumber) {
	//Reseteamos todos los pines
	GPIO_WritePin(&handlerPA6, RESET);
	GPIO_WritePin(&handlerPB8, RESET);
	GPIO_WritePin(&handlerPB9, RESET);
	GPIO_WritePin(&handlerPA7, RESET);
	GPIO_WritePin(&handlerPA6, RESET);
	GPIO_WritePin(&handlerPC0, RESET);
	GPIO_WritePin(&handlerPC1, RESET);
	GPIO_WritePin(&handlerPB0, RESET);

	switch (PinNumber) {

	case 0: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA7, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPC0, SET);
		GPIO_WritePin(&handlerPC1, SET);
		GPIO_WritePin(&handlerPB9, SET);
		break;
	}

	case 1: {
		GPIO_WritePin(&handlerPB9, SET);
		GPIO_WritePin(&handlerPB0, SET);
		break;
	}

	case 2: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA6, SET);
		GPIO_WritePin(&handlerPB9, SET);
		GPIO_WritePin(&handlerPC1, SET);
		GPIO_WritePin(&handlerPC0, SET);
		break;
	}

	case 3: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA6, SET);
		GPIO_WritePin(&handlerPB9, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPC0, SET);
		break;
	}

	case 4: {
		GPIO_WritePin(&handlerPA7, SET);
		GPIO_WritePin(&handlerPB9, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPA6, SET);
		break;
	}

	case 5: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA7, SET);
		GPIO_WritePin(&handlerPA6, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPC0, SET);
		break;
	}

	case 6: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA6, SET);
		GPIO_WritePin(&handlerPC1, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPC0, SET);
		GPIO_WritePin(&handlerPA7, SET);

		break;
	}

	case 7: {
		GPIO_WritePin(&handlerPB9, SET);
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPB0, SET);

		break;
	}

	case 8: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA7, SET);
		GPIO_WritePin(&handlerPA6, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPC0, SET);
		GPIO_WritePin(&handlerPC1, SET);
		;
		GPIO_WritePin(&handlerPB9, SET);
		break;
	}

	case 9: {
		GPIO_WritePin(&handlerPB8, SET);
		GPIO_WritePin(&handlerPA7, SET);
		GPIO_WritePin(&handlerPA6, SET);
		GPIO_WritePin(&handlerPB0, SET);
		GPIO_WritePin(&handlerPC0, SET);
		GPIO_WritePin(&handlerPB9, SET);

		break;
	}
	// Falta el caso default...

	}
}

//Esta funcion muestra los numeros en pantalla
void displayNumbers(char num) {

//______________________________________________________
//Primera Parte: Transformar el numero a un arreglo de 2 elementos

	if (num < 10) {
		NUM[1] = 0;    // borrar el numero de la izquierda
		NUM[0] = num;  //añadir a la pantalla derecha el numero del contador
	} else {
		if (m < 2) {
			while (num != 0) {
				digit = num % 10; //descomponemos el numero en 2 digitos, usando su residuo al dividirlo con 10
				num = num / 10; //Repetimos el procesotantas veces como el numero no sea 0
				NUM[m] = digit; // Los valores los vamos guardando en el arreglo NUM, primero para la posicion 0, y luego la 1.
				m = m + 1;
			}
		} else {
			m = 0;
		}
	}
//________________________________________________________________________
//Segunda parte: Despliego los valores en pantalla

	if (n < 2) {
		if (callback3flag == 1) {
			callback3flag = 0;
			GPIOxTooglePin(&handler2);
			GPIOxTooglePin(&handler3); //cambiamos de estado ambas pantallas asincronamente
			numbers(NUM[n]);   //A su vez le asignamos a cada pantalla un digito
			n = n + 1;
			// Esta forma de actualizar los datos hace que se presente una sombra en los números
			// En tu caso es muy tenue pero se puede apreciar.
		}
	}

	else {
		n = 0;
	}
}

void GPIO_PinFastConfig(GPIO_Handler_t *pPinHandler, uint8_t PINX,	GPIO_TypeDef *pGPIOx) {
	pPinHandler->pGPIOx = pGPIOx;
	pPinHandler->GPIO_PinConfig.GPIO_PinNumber = PINX;
	pPinHandler->GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	pPinHandler->GPIO_PinConfig.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	pPinHandler->GPIO_PinConfig.GPIO_PinAltFunMode = AF0;
	// muy interesante esta funcion.
}
