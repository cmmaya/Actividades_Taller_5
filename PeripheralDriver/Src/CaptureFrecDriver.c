/*
 * CaptureFrecDriver.c
 */

#include "CaptureFrecDriver.h"
#include "BasicTimer.h"


uint32_t timeStamp;

void capture_Config(Capture_Handler_t *ptrCaptureHandler){
	__disable_irq(); // se desactiva las interrupciones
	//1. Activar la senal de reloj del periferico requerido
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}
	/* 2a. Estamos en UP mode, el liite se carga en ARR y se comienza en 0*/
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;
	/*Configuramos los bits CCxS del registo TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto de CCxS)
	 *
	 * 4. Ademas, en el mismo "case: podemos confgurar el modo PWM, su polaridad,
	 *
	 * 5. Y ademas activamos el preload bit, para que cada vez qu exista un update ecent
	 * el valor cargado en el CCRx sera recargado en el registro shadow*/
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);
		//Configuramos como entada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1<<TIM_CCMR1_CC1S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture<< TIM_CCMR1_IC1PSC_Pos);

		//configuramos el flanco qe deseamos cpturar
		if(ptrCaptureHandler ->config.edgeSignal == CAPTURE_RISING_EDGE){
			//configuramos 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		//Activamos el modulo de captura
//		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;
		break;
	}
	case CAPTURE_CHANNEL_2:{
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);
		//Configuramos como entada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1<<TIM_CCMR1_CC2S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture<< TIM_CCMR1_IC2PSC_Pos);

		//configuramos el flanco qe deseamos cpturar
		if(ptrCaptureHandler ->config.edgeSignal == CAPTURE_RISING_EDGE){
			//configuramos 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		//Activamos el modulo de captura
//		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;
		break;
	}
	case CAPTURE_CHANNEL_3:{
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);
		//Configuramos como entada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (1<<TIM_CCMR2_CC3S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC3F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 << TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture<< TIM_CCMR2_IC3PSC_Pos);

		//configuramos el flanco qe deseamos cpturar
		if(ptrCaptureHandler ->config.edgeSignal == CAPTURE_RISING_EDGE){
			//configuramos 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		//Activamos el modulo de captura
//		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;
		break;
	}
	case CAPTURE_CHANNEL_4:{
		//Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR2_CC4S);
		//Configuramos como entada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1<<TIM_CCMR2_CC4S_Pos);

		//Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR2_IC4F_Pos);

		//Configuramos el prescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture<< TIM_CCMR2_IC4PSC_Pos);

		//configuramos el flanco qe deseamos cpturar
		if(ptrCaptureHandler ->config.edgeSignal == CAPTURE_RISING_EDGE){
			//configuramos 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		else{
			//Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		//Activamos el modulo de captura
//		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
		break;
	}
	default:{
		__NOP();
		break;
	}

	}//fin del switch case

	//Configuramos el prescaler
	ptrCaptureHandler->ptrTIMx->PSC =ptrCaptureHandler->config.timerSpeed;


	/*Configuramos las interupciones: le decimos que se levante la bandera de que la interrupcion se dio segun el canal
	Que elegimos, Dicha interrupcion esta matriculada en el DIER de cada timer. Por tanto la interrupcion va a ser
	atendida en el IRQ del timer en cuestion*/
	if(ptrCaptureHandler->config.enableInt == INTERRUPT_ENABLE){
		switch(ptrCaptureHandler->config.channel){
		case CAPTURE_CHANNEL_1:{
			//Matriculammos la interrupcion del canal 1
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC1IE;
			break;
		}
		case CAPTURE_CHANNEL_2:{
			//Matriculammos la interrupcion del canal 2
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC2IE;
			break;
		}
		case CAPTURE_CHANNEL_3:{
			//Matriculammos la interrupcion del canal 3
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC3IE;
			break;
		}
		case CAPTURE_CHANNEL_4:{
			//Matriculammos la interrupcion del canal 4
			ptrCaptureHandler->ptrTIMx->DIER |= TIM_DIER_CC4IE;
			break;
		}
		default:{
			break;
		}
		}
	}
	else{
		__NOP();
	}

	/* Activamos el canal del sistema NVIC para que lea la interrupción*/
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		// Activando en NVIC para la interrupción del TIM2
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM4_IRQn);
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		// Activando en NVIC para la interrupción del TIM3
		NVIC_EnableIRQ(TIM5_IRQn);
	}
	else{
		__NOP();
	}
	//activamos las interrupciones
	__enable_irq();
}



/*Funcion para inicializar el conteo y la captura*/
void startCapture(Capture_Handler_t *ptrCaptureHandler){


	//reiniciamos el contador del timer
	ptrCaptureHandler->ptrTIMx->CNT = 0;

	switch (ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR1 = 0;

		//Bajamos la bandera que indica que existe un evento de captura, y el overrun por si acaso
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1OF;

		//INICIAMOS EL CONTEO
		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;
		//Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;


		break;
	}
	case CAPTURE_CHANNEL_2:{
		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR2 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2OF; // necesario?

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;


		break;
	}
	case CAPTURE_CHANNEL_3:{


		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3OF;

		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		break;
	}
	case CAPTURE_CHANNEL_4:{


		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3OF; // necesario?

		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

	}
	default:{
		break;
	}
	}

}

//////////////////////////////////////
//ESTA ES LA FUNCION VIEJA
/////////////////////////////////////


/*Esta funcion se encarga de lanzar la captura de la frecuencia... en este caso funciona
 * con pulling, examinando cuando se levanta la bandera del evento de captura. */

uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp1 = 0;
	uint32_t timestamp2 = 0;
	uint32_t deltaTimestamp = 0;

	//reiniciamos el contador del timer
	ptrCaptureHandler->ptrTIMx->CNT = 0;

	/*Todo lo que sigue de aca en adelante depende del canal seleccionado,
	 * por lo cual se escribe cada codigo dentro del "case" escifico.
	 * Antes de lanzar la captua (encender e TImer para que cuente)
	 * debemos norrar el valor de los registros CCP, de forma que arranquen limpios
	 */
	switch (ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR1 = 0;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |=TIM_CR1_CEN;

		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)){}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR1;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;

		//Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Esperemos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)) {}

		//Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		//Caturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la primer
		//captura
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR1;
		deltaTimestamp = timestamp2 - timestamp1;
		break;
	}
	case CAPTURE_CHANNEL_2:{
		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR2 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |=TIM_CR1_CEN;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;

		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)){}

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR2;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;

		//Esperemos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)) {}

		//Caturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la primer
		//captura
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR2;
		deltaTimestamp = timestamp2 - timestamp1;
		break;
	}
	case CAPTURE_CHANNEL_3:{
		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |=TIM_CR1_CEN;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;

		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)){}

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR3;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;

		//Esperemos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)) {}

		//Caturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la primer
		//captura
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR3;
		deltaTimestamp = timestamp2 - timestamp1;
		break;
	}
	case CAPTURE_CHANNEL_4:{
		//Borramos el valor inicial que indica un evento e captura
		ptrCaptureHandler->ptrTIMx->CCR4 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |=TIM_CR1_CEN;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

		//Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)){}

		//Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR4;

		//Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

		//Esperemos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)) {}

		//Caturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la primer
		//captura
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR4;
		deltaTimestamp = timestamp2 - timestamp1;
		break;
	}
	default:{
		break;
	}
	}
	return deltaTimestamp;
}
/////////////////////////////////////////////////////////////////////


void stopCapture(Capture_Handler_t *ptrCaptureHandler){
	ptrCaptureHandler->ptrTIMx->CNT = 0;
	switch (ptrCaptureHandler->config.channel) {
		case CAPTURE_CHANNEL_1:{
			//apagamos la captura y luego el timer
			ptrCaptureHandler->ptrTIMx->CCER &= ~TIM_CCER_CC1E;
			ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
			//Bajamos la bandera que indica que existe un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;
			break;
		}
		case CAPTURE_CHANNEL_2:{
			ptrCaptureHandler->ptrTIMx->CCER &= ~TIM_CCER_CC2E;
			ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
			//Bajamos la bandera que indica que existe un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;

			break;
		}
		case CAPTURE_CHANNEL_3:{
			ptrCaptureHandler->ptrTIMx->CCER &= ~TIM_CCER_CC3E;
			ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
			//Bajamos la bandera que indica que existe un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;
			break;
		}
		case CAPTURE_CHANNEL_4:{
			ptrCaptureHandler->ptrTIMx->CCER &= ~TIM_CCER_CC4E;
			ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
			//Bajamos la bandera que indica que existe un evento de captura
			ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;
			break;
		}
		default:{
			break;
		}

}
}


//Creamos el Callback de la captura del TIMER 4: Aqui viene a ser atendida la interrupcion una vez efectuada.
__attribute__((weak)) void TimerCapture4_Callback(void){
__NOP();

}

