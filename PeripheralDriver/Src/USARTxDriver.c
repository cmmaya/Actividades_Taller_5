/*
 * USARTxDriver.c
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

uint8_t auxRxData = 0;

/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){
	__disable_irq();
	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periferico */
	/* Lo debemos hacer para cada uno de las pisbles opciones que tengamos (USART1, USART2, USART6) */
    /* 1.1 Configuramos el USART1 */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		// Escriba acá su código
	}
	
    /* 1.2 Configuramos el USART2 */
    // Escriba acá su código
	else if(ptrUsartHandler->ptrUSARTx == USART2){
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		// Escriba acá su código
	}


    /* 1.3 Configuramos el USART6 */
	else if(ptrUsartHandler->ptrUSARTx == USART6){
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
		// Escriba acá su código
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */

	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
    // Tenga cuidado, el parity hace parte del tamaño de los datos...
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PS;
			
		}else{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;
		}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE;

	}

	// 2.3 Configuramos el tamaño del dato
	if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M;
	}
	else{
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;

	}

	//Limpiamos los STOPBITS
	ptrUsartHandler->ptrUSARTx->CR2 &= ~ USART_CR2_STOP;

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemoscargar el valor 0b00 en los dos bits de STOP
		// Escriba acá su código
		__NOP();
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemoscargar el valor 0b01 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 |=  USART_CR2_STOP_0;
		break;
	}
	case USART_STOPBIT_2: {
		// Debemoscargar el valor 0b10 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
		break;


	}
	case USART_STOPBIT_1_5: {
		// Debemoscargar el valor 0b11 en los dos bits de STOP
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP;
		break;
	}
	default: {
		// En el casopor defecto seleccionamos 1 bit de parada
		// Escriba acá su código
		__NOP();
		break;
	}
	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;
		if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_9600){
			// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
			// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
			// Valor a cargar 0x0683
			// Configurando el Baudrate generator para una velocidad de 9600bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x0683; // para 16Mhz
//			ptrUsartHandler->ptrUSARTx->BRR = 0xa2c;  // para 100Mhz

		}

		else if (ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_19200) {
			// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
			// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
			// Escriba acá su código y los comentarios que faltan
			ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
		}

		else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_115200){
			// Escriba acá su código y los comentarios que faltan
			// El valor a cargar es 54.2535 -> Mantiza = 54,fraction = 0.2535*16 = 4
			ptrUsartHandler->ptrUSARTx->BRR = 0x008b; //Para 16Mhz
//			ptrUsartHandler->ptrUSARTx->BRR = 0x01B2; //Para 100Mhz

		}

		else if(ptrUsartHandler->USART_Config.USART_baudrate == USART_BAUDRATE_57600){
			// Escriba acá su código y los comentarios que faltan
			ptrUsartHandler->ptrUSARTx->BRR = 0x116;
		}


	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable

	//limpiamos el registro
	ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
	ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;

	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}
	
	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		// Escriba acá su código
		break;
	}
	}


	//2.8 Verificamos la configuracion de las interrupciones
    //2.8a Interrupcion por recepcion
	if(ptrUsartHandler->USART_Config.USART_enableIntRX == USART_RX_INTERRUPT_ENABLE){
		//Como esta activada, debemos configurar la interrupcion por recepcion
		/*Debemos activar =la interupcion RX en la configuracion del USART */
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;

		/*Debemos matriuclar la interrupcion en el NVIC */
		/* Lo debemos hacer para cada uno de las posibles opciones que tengamos  (USART1, USART2, USART6) */
		if(ptrUsartHandler -> ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
		}
		else if(ptrUsartHandler -> ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
		}
		else if(ptrUsartHandler -> ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
		}

}

	else{
		__NOP();
	}
	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		// Escriba acá su código
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;
	}
	__enable_irq();
}


/* funcion para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}
	ptrUsartHandler->ptrUSARTx->DR = (char) dataToSend;

	// Escriba acá su código

	return dataToSend;
}

/**/
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend){
	while(*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

uint8_t getRxData(void){
	return auxRxData;
}

/*Handler de la interrupcion del USART
 * Aca deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
__attribute__((weak)) void usart1Rx_Callback(void){
	  /* NOTE : This function should not be modified*/

	__NOP();
}

__attribute__((weak)) void usart2Rx_Callback(void){
	  /* NOTE : This function should not be modified*/

	__NOP();
}
__attribute__((weak)) void usart6Rx_Callback(void){
	  /* NOTE : This function should not be modified*/
	__NOP();
}


void USART1_IRQHandler(void){
	//Evaluamos si la interrucion que se dio es RX

	if(USART1->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1->DR;
		usart1Rx_Callback();
	}
}
void USART2_IRQHandler(void){
	//Evaluamos si la interrucion que se dio es RX

	if(USART2->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2->DR;
		usart2Rx_Callback();
	}
}
void USART6_IRQHandler(void){
	//Evaluamos si la interrucion que se dio es RX

	if(USART6->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6->DR;
		usart6Rx_Callback();
	}
}

/*
 * Camilo,
 * Lo primero es volver a ahcerte la observacion sobre poner comentarios en el código, vos no lo haces o solo
 * pones alguna palabra simple... creeme que uno pone comentarios para el futuro.
 * Con respecto a la configuración, hay varios problemas:
 * - Los valores para el trabajo con el PLL estaán comentados. Reralmente deberias tener una forma de seleccionar los
 *   valores con respecto a la velocidad que se tiene seleccionada.
 * - Hay una mezcla confusa entre valores para 16MHz y 100MHz.
 * - No tienes en cuenta que el bus APB1 (USART1 y USART6) corre mas rapido que el APB2 (USART2), por lo cual toda tu
 *   configuracion queda solo sirviendo para el USART2, los otros dos dejan de funcionar.
 * Calificación = 3.1
 * */



