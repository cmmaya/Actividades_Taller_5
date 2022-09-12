
#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"



uint32_t i = 0;


int main(void){

	GPIO_Handler_t handlerUserButton = {0};


	// Aqui declaramos las propiedades del boton
	handlerUserButton.pGPIOx = GPIOC;                                              //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;              //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;        //tipo salida
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;  //pull up pull down
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				   //Ninguna función.




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


