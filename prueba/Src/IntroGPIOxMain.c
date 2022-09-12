
#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"



uint32_t i = 0;
int a;

int main(void){

	GPIO_Handler_t handlerUserButton = {0};  //la estructura que le da las prop al boton


	// Aqui declaramos las propiedades del boton
	handlerUserButton.pGPIOx = GPIOC;                                              //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;              //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;        //tipo salida
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
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
	GPIO_Config(&handlerUserButton);

//////  PUNTO 2 /////

	 //Aqui definimos la funcion: le entra una variable tipo GPIO_Handler_t,que carga con la configuracion del pin (Todas sus propiedades)
	void GPIOxTooglePin(GPIO_Handler_t *handlerUserPin){

   /*
    * En este apartado cambiamos el estado de output del PIN, dicha informacion se encuentra en el ODR,
      para cambiar su estado, invocamos la propiedad del ODR de nuestro PIN (handlerUserPin->pGPIOx->ODR)

      dica direccion nos da el registr de memoria del ODR (el valor de los 16 bits), y para cambiar unicamente
      el PIN que queremos utilzamos una operacion XOR, como se ve en el ejemplo

      0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0        --> el pin 5 esta activado
      0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 XOR    --> aplicamos la mascara XOR en la posicion que deseamos
    = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

     nota: si por el contrario el pin estuviera apagado la operaicion XOR lo prende = 0^1 = 1


    *Lo siguiente es modificar la mascara para que el bit 1 este en la misma posicion del PIN que queremos,
     para eso usamos el shift a la izquierda: 1<<handlerUserPin->GPIO_PinConfig.GPIO_PinNumber
     Le estamos diciendo que al bit 1, lo desplace a la izquierda tantas veces como el numero del pin:
     1<<5 = 100000
    */
		handlerUserPin->pGPIOx->ODR = handlerUserPin->pGPIOx->ODR ^ (1<<handlerUserPin->GPIO_PinConfig.GPIO_PinNumber);
	}


/////PUNTO 3////////

	//aqui se ejecuta la funcion principal
	while(1){

		//le decimos al programa que si lee un cambio en el boton, cambie el estado del LED
		if(GPIO_ReadPin(&handlerUserButton )==0){
			GPIOxTooglePin(&handlerUserLedPin);   //con la funcion toogle cambiamos el estado del PIN
		}


		//La funcion de a continuacion genera un retraso entre los pulsos, pues sin esta funcion, cuando se pulsa el boton el microcontrolar procesa miles de pulsos.
		for(i=0;i<200000;i++){
			a=1+1;
		}

     /* Lo que hace el ciclo for es ejecutar procesos que retrasan que el primer condicional se active de nuevo, pues la capacidad del mirocontrolador hace que se
      * detecten miles de pulsos en un segundo (esto hace que cuando se pulse el boton ocuran cosas extranas). Por lo anterior se debe esperar lo que dure en procesar
      *  el ciclo para volver a activar el boton.
      */
	}

}
