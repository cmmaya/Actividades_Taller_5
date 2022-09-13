/////////////////PUNTO 1 ///////////////
/*

 * En el archivo "GPIOxDriver.c" Se modificó la funcion GPIO_ReadPin:

 - La funcion se le introduce una variable tipo GPIO_Handler y es un puntero (por el asterisco)
 - Internamente lo que hace es tomar las posiciones de memoria en IDR y desplazarlas hacia la derecha
   tantas veces como el numero del PIN. con el comando 	pinValue = (pPinHandler->pGPIOx->IDR>>pPinHandler->GPIO_PinConfig.GPIO_PinNumber);
   esta informacion la guarda en la variable "pinValue".


    Por ejemplo, si quiero obtener el PIN 4(que esta encendido), la posicion de memoria se ve asi:
   	 * 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0   --> posicion inicial
	 * 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1   --> Posicion desplazada hacia la derecha 4 veces

 - ahora, como yo solo quiero saber el valor de la variable, y no los valores de la memoria IDR en general, aplico una mascara
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
	return pinValue;
	}
 */



#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"



uint32_t i = 0;

//////  PUNTO 2 /////

	 //Aqui definimos la funcion Toogle: le entra una variable tipo GPIO_Handler_t,que carga con la configuracion del pin (Todas sus propiedades)
	void GPIOxTooglePin(GPIO_Handler_t *handlerUserPin){

   /*
    * En este apartado cambiamos el estado de output del PIN, dicha informacion se encuentra en el ODR.
      Para cambiar su estado, invocamos la propiedad del ODR de nuestro PIN (handlerUserPin->pGPIOx->ODR)

      dica direccion nos da el registro de memoria del ODR (el valor de los 16 bits), y para cambiar unicamente
      el PIN que queremos, utilzamos una operacion XOR, como se ve en el ejemplo



      0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0        --> Memoria en ODR, el pin 5 esta activado
      0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 XOR    --> aplicamos la mascara XOR en la posicion que deseamos
    = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

     nota: si por el contrario el pin estuviera apagado la operaicion XOR lo prende = 0^1 = 1


    *Lo siguiente es modificar la mascara para que el bit 1 de la mascara este en la misma posicion del PIN que queremos,
     para eso usamos el shift a la izquierda: 1<<handlerUserPin->GPIO_PinConfig.GPIO_PinNumber
     Le estamos diciendo que al bit 1, lo desplace a la izquierda tantas veces como el numero del pin:
     000001<<5 = 100000
    */
		handlerUserPin->pGPIOx->ODR = handlerUserPin->pGPIOx->ODR ^ (1<<handlerUserPin->GPIO_PinConfig.GPIO_PinNumber);
	}



int main(void){

	GPIO_Handler_t handlerUserButton = {0};  //la estructura que le da las prop al boton interno
	GPIO_Handler_t handlerUserButton1 = {0}; //la estructura que le da las prop al boton externo

	// Aqui declaramos las propiedades del boton interno
	handlerUserButton.pGPIOx = GPIOC;                                              //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;              //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;        //tipo entrada
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				   //Ninguna función.
	// Aqui declaramos las propiedades del boton externo
	handlerUserButton1.pGPIOx = GPIOC;                                             //numero de PUERTO
	handlerUserButton1.GPIO_PinConfig.GPIO_PinNumber		= PIN_6;               //Numero del PIN
	handlerUserButton1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;        //tipo entrada
	handlerUserButton1.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLDOWN; //pull down
	handlerUserButton1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserButton1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;				   //Ninguna función.



	GPIO_Handler_t handlerUserLedPin1 = {0}; //la estructura que le da las prop al pin PC10
	GPIO_Handler_t handlerUserLedPin2 = {0}; //la estructura que le da las prop al pin PC11
	GPIO_Handler_t handlerUserLedPin3 = {0}; //la estructura que le da las prop al pin PC12
	GPIO_Handler_t handlerUserLedPin  = {0}; //la estructura que le da las prop al led interno

	// Aqui declaramos las propiedades del LED Interno
	handlerUserLedPin.pGPIOx = GPIOA;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;       //salida
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					//Ninguna función.

	// Aqui declaramos las propiedades del LED 1
	handlerUserLedPin1.pGPIOx = GPIOC;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;       //salida
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserLedPin1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					//Ninguna función.

	// Aqui declaramos las propiedades del LED 2
	handlerUserLedPin2.pGPIOx = GPIOC;
	handlerUserLedPin2.GPIO_PinConfig.GPIO_PinNumber		= PIN_11;
	handlerUserLedPin2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;       //salida
	handlerUserLedPin2.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserLedPin2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserLedPin2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserLedPin2.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					//Ninguna función.

	// Aqui declaramos las propiedades del LED 3
	handlerUserLedPin3.pGPIOx = GPIOC;
	handlerUserLedPin3.GPIO_PinConfig.GPIO_PinNumber		= PIN_12;
	handlerUserLedPin3.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;       //entrada o salida
	handlerUserLedPin3.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserLedPin3.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserLedPin3.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserLedPin3.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;					//Ninguna función.


	//Cargamos la configuracion
	GPIO_Config(&handlerUserLedPin1);
	GPIO_Config(&handlerUserLedPin2);
	GPIO_Config(&handlerUserLedPin3);
	GPIO_Config(&handlerUserLedPin);
	GPIO_Config(&handlerUserButton);
	GPIO_Config(&handlerUserButton1);


/////PUNTO 3////////

	//aqui se ejecuta la funcion principal
	while(1){


		//le decimos al programa que si lee un cambio en el boton con ReadPin (que recoge los datos de el PIN en el IDR), cambie el estado del LED
		if(GPIO_ReadPin(&handlerUserButton )==0){
			GPIOxTooglePin(&handlerUserLedPin); //Para cambiar el estado del PIN usamos la funcion previamente definida
			//La funcion de a continuacion genera un retraso entre los pulsos.
			for(i=0;i<200000;i++){
				NOP();
			}
		}

	     /* Lo que hace el ciclo for es ejecutar procesos que retrasan que el primer condicional se active de nuevo, pues la capacidad del mirocontrolador hace que se
	      * detecten miles de pulsos en un segundo (esto hace que cuando se pulse el boton ocurran cosas extranas). Por lo anterior se debe esperar lo que dure en procesar
	      *  el ciclo para volver a activar el boton.
	      */



/////PUNTO 4//////


		//Le decimos al programa que si se ejecuta el boton1 (el externo) se inicie el ciclo
		if(GPIO_ReadPin(&handlerUserButton1)){

			//Primero se prenden los 3 leds simultaneamente (Aunque tecnicamente no sea simultanea la instruccion, asi lo percibimos)
			GPIOxTooglePin(&handlerUserLedPin1);
			GPIOxTooglePin(&handlerUserLedPin2);
			GPIOxTooglePin(&handlerUserLedPin3);


			/*Con los ciclos for retrasamos el inicio de la siguiente instruccion, asi vamos diciendole al programa que apague los leds
			   progresivamente.
			 * Se encontró que poner a ejecutar el ciclo 4000000 veces genera un restraso de aprox 5s
			 */

			for(i=0;i<4000000;i++){
				NOP();
			}
			GPIOxTooglePin(&handlerUserLedPin3);  //Se apaga el PIN_12


			//el siguiente es 2/5 de 5s, osea 2 seg
			for(i=0;i<1600000;i++){
				NOP();
			}
			GPIOxTooglePin(&handlerUserLedPin2);  //Se apaga el PIN_11


			//el siguiente es 1/5 de 5s, osea 1 seg
			for(i=0;i<800000;i++){
				NOP();
			}
			GPIOxTooglePin(&handlerUserLedPin1);  //Se apaga el PIN_10

		}



	}

}
