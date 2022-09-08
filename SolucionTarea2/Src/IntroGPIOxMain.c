/////////////////PUNTO 1 ///////////////
/*

 * En el archivo "GPIOxDriver" Se modificó la funcion GPIO_ReadPin:

 - La funcion se le introducia una variable tipo GPIO_Handler y es un puntero (por el asterisco)
 - Internamente lo que hace es tomar las posiciones de memoria en IDR y desplazarlas hacia la derecha
   tantas veces como el numero del PIN. con el comando 	pinValue = (pPinHandler->pGPIOx->IDR>>pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
   esta informacion la guarda en la variable pinValue


    Por ejemplo, si quiero obtener el PIN 4(que esta encendido), la posicion de memoria se ve asi:
   	 * 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0   --> posicion inicial
	 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1   --> Posicion desplazada hacia la derecha 4 veces

 - ahora, como yo solo quiero saber el valor de la variable, y no los valores de la memoria ODR en general, aplico una mascara
   al ultimo resultado, con el objetivo de que me diga si el bit de la derecha esta prendido o apagado.
   Esto lo logro con la operacion AND (&)

   	pinValue = pinValue & 0b1;
   	Aqui hay un ejemplo de como funciona la mascara AND:

   	0 0 0 0 1 0 0 0 1 0 0 0 0 0 0 1
   	0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1  &
  = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1   =  1


   La mascara anula todos lo terminos menos el ultimo, que lo vuelve 1, si es 1 y lo vuelve 0 si es 0.


   - Al final se retorna la variable pinValue modificada, que nos dara solo el valor del bit que requerimos


   A continuacion se muestra el codigo completo:

uint32_t GPIO_ReadPin(GPIO_Handler_t *pPinHandler){

	uint32_t pinValue = 0;


	pinValue = (pPinHandler->pGPIOx->IDR>>pPinHandler->GPIO_PinConfig.GPIO_PinNumber);


	pinValue = pinValue & 0b1;

	pinValue = (pPinHandler->pGPIOx->IDR>>pPinHandler->GPIO_PinConfig.GPIO_PinNumber);

	return pinValue;
	}
 */




///////PUNTO 2///////////
#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"



uint32_t i = 0;


int main(void){

	/*
	 * para ensayar si la funcion sirve, vamos a configurar un PIN cualquiera
	 */

	GPIO_Handler_t handlerUserLedPin = {0};  //inicializamos la estructura que le da las prop al pin

	// Aqui declaramos las propiedades del PIN
	handlerUserLedPin.pGPIOx = GPIOA;                                              //numero de PUERTO
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;               //Numero del PIN
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;       //tipo salida
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				   //Ninguna función.

	//Cargamos la configuracion del PIN especifico
	GPIO_Config(&handlerUserLedPin);

	//Hacemos que el PIN_A5 quede encendido
	GPIO_WritePin(&handlerUserLedPin, SET);


// Aqui se ejecuta la funcion, si el pin estaba prendido, se apaga. Y vicersa

	void GPIOxTooglePin(GPIO_Handler_t *handlerUserPin){


		handlerUserPin->pGPIOx->ODR = handlerUserPin->pGPIOx->ODR ^ (1<<handlerUserPin->GPIO_PinConfig.GPIO_PinNumber);
	}

	GPIOxTooglePin(&handlerUserLedPin);


	while(1){

	}

}


