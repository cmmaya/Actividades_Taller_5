
#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"


	char n            = 0;  //contador para el ciclo for
	char bandera      = 0;  //variable bandera, para avisar cuando se ejecutó la interrupcion
	int dataToSend    = 1;  //dato para mandar por USART
	int dataArray[10] = {'h','o','l','a',' ','m','u','n','d','o'};  //arreglo para mandar or USART


	/*Definimos los arreglos para linkearles las configuraciones de los perifericos*/
GPIO_Handler_t       handlerTxPin         = {0};
GPIO_Handler_t       handlerBlinky        = {0};
GPIO_Handler_t       handlerUserButton    = {0};
USART_Handler_t      USARTtransmision     = {0};
BasicTimer_Handler_t handlerTimer2        = {0};





int main(void){
	//Config el Transmission Pin
	handlerTxPin.pGPIOx                                    = GPIOA;               //Puerto A
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber             = PIN_9;               //Pin 9 (se extrajo del AltFunct en el ref manual)
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode               = GPIO_MODE_ALTFN;     //Se activa la funcion ALTFN del pin especifico
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType             = GPIO_OTYPE_PUSHPULL; //tipo pushpull
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed	           = GPIO_OSPEED_FAST;    //velocidad
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl        = GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode         = AF7;                 //Se escoge la funcion alternativa correspondiente
    GPIO_Config(&handlerTxPin);


	//Config el Blinky
	handlerBlinky.pGPIOx                                   = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber            = PIN_5;               //pin A5 (el led verde del fabricante)
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode              = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType            = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed	           = GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	   = GPIO_PUPDR_NOTHING;
    GPIO_Config(&handlerBlinky);

	//Config el Boton
	handlerUserButton.pGPIOx = GPIOC;                                              //numero de PUERTO
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;              //Numero del PIN
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;        //tipo entrada
	handlerUserButton.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL; //defino que es pushpull
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;  //pull up pull down
	handlerUserButton.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;  // vel media
	handlerUserButton.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;				   //Ninguna función.
    GPIO_Config(&handlerUserButton);


	//Configuro el usart
	USARTtransmision.ptrUSARTx                              = USART1;              //Usamos el USART1
	USARTtransmision.USART_Config.USART_mode                = USART_MODE_TX;       //Modo transimison
	USARTtransmision.USART_Config.USART_baudrate            = USART_BAUDRATE_9600; //Usamos un baudrate de 9600(9598)
	USARTtransmision.USART_Config.USART_datasize            = USART_DATASIZE_8BIT; //Usamos 8 bits (no queremos paridad)
	USARTtransmision.USART_Config.USART_parity              = USART_PARITY_NONE;   //Paridad desactivada
	USARTtransmision.USART_Config.USART_stopbits            = USART_STOPBIT_1;     //Stopbit de 1
	USART_Config(&USARTtransmision);

    //Configuro El Timer
	handlerTimer2.ptrTIMx                                   = TIM2;                //Selecciono el Timer 2
	handlerTimer2.TIMx_Config.TIMx_mode                     = BTIMER_MODE_UP;      //Conteo ascendente
	handlerTimer2.TIMx_Config.TIMx_speed                    = BTIMER_SPEED_100us;  //velocidad del conteo
	handlerTimer2.TIMx_Config.TIMx_period                   = 2500;  //250ms       //periodo de la interrupcion
	BasicTimer_Config(&handlerTimer2);




//aqui ejecuto la funcion
while(1){

	/*usamos la bandera para indicar que la interrupcion del timer se ejecutó.
	 * Si la interrupcion se ejecuta, entonces procedo a correr el codigo, y al
	 * final del ciclo, se apaga la bandera para ser activada en la siguiente in-
	 * terrupcion
	 */

	if (bandera == 1 && GPIO_ReadPin(&handlerUserButton) == 1){
		/*Si la bandera esta activada y el boton NO esta presionado, entonces mande
		 * un dato cada periodo de la interrupcion (cada que la bandera sea = 1)*/

		writeChar(&USARTtransmision, dataToSend);
		bandera = 0; //apago la bandera despues de mandar el dato
	}

	if (bandera == 1 && GPIO_ReadPin(&handlerUserButton) == 0){
		/*En el caso que la bandera este activada y el boton tambien, entonces comience
		 * a mandar los datos del Arreglo */

		writeChar(&USARTtransmision, dataArray[n]); //mando el primero dato del arreglo

		/*al mandar el primer dato (n=0), cambio el valor de n a n+1, para que en el si-
		 * guiente ciclo envie el dato siguiente. Este valor se reinicia cuando se llega
		 * a la ultima posicion del arreglo (para comenzar de nuevo el ciclo).*/
		if(n<10){
			n +=1;
			}
		else{
			n=0;
			}
		bandera = 0; //Al final apagamos la bandera

	}

	}

return 0;
}


// callback de la interrupcion del Timer2
void BasicTimer2_Callback(void){
	bandera = 1;                       //prendemos la bandera cada que se generala interrupcion
	GPIOxTooglePin(&handlerBlinky);    //Hacemos al Blinky parpadear cada 250 ms

}

