/*
 * IntroGPIOxMain.c
 *
 *  Created on: Aug 25, 2022
 *      Author: Juan Pablo Rengifo Ruiz
 ****************************************************************************************
 *
 *
 * Con este programa se desea mostrar el uso basico de los registros que controlan
 * al micro (SFR) y la forma adecuada para utilziar los operadores &, |, ~ y =, para
 *  cambiar la configuracion de algun registro.
 *  Tambien es importante para entender la utilidad de los numeros BIN y HEX
 *
 *  Es necessario tener a la mano tanto el manual de referencia del micro como la hoja de
 *  datos del micro (ambos son documentos diferentes y complementarios el uno del otro)
 *
 *  HAL -> Hardware Abstraction Layer.
 *
 *  Este programa introduce el periférico mas simple que tiene el mico, que es el
 *  encargado de manejar los pines de cada puerto del micro.
 *
 *  Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo entrata
 *  y salida de datos digitales (o sea 1's y 0's), además se les puede asignar funciones
 *  especiales que generalmente estan ligadas a otro periférico adicional (se verá mas
 *  adelante).
 *
 *  De igual forma, varias caracteristicas pueden ser configuradas para cada el PINx
 *  especifico como son:
 *  - La velocidad a la que puede responder
 *  - Tipo de entrada (pull up, pull down, open drain (flotante))
 *  - Tipo de salida (push-pull, Open drain ...)
 *  - Entrada análoga
 *
 *  Debemos definir entonces todos los registros que manejan el periferico GPIOx y luego
 *  crear algunas funciones para utilizar adecuadamente el equipo.
 *
 ********************************************************************************************
 */

#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"
uint32_t i = 0;

/* Función principal del programa. Es acá donde se ejecuta toto */
// con asterisco es la posicion y sin asterisco es el valor de la variable
int main(void){

	//*********
	// Definimos el handler para el PIN que deseamos configurar

	GPIO_Handler_t handlerUserLedPin = {0};  //la estructura que le da las prop al pin

	// Deseamos trabajar con el puerto GPIOA
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;       //entrada o salida
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna función.

	//Cargamos la configuracion del PIN especifico
	GPIO_Config(&handlerUserLedPin);

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, RESET);

	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

	}
}


