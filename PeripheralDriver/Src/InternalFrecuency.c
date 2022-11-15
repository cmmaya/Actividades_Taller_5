/*
 * InternalFrecuency.c
 */
#include "InternalFrecuency.h"

void internalFrecuency_config(void){
//	RCC->CR |= RCC_CR_HSEON;  //Activamos el HSE para que el reloh no funcione en el HSI y poder configurar el PLL
//	while(!(RCC->CR & RCC_CR_HSERDY));
//
//	RCC ->CR &= ~RCC_CR_HSION; //Desactivamos el HSI para poder configurar el PLL en el
//	while((RCC->CR & RCC_CR_HSIRDY));

	//2. Configuramos la fuente input como HSI
	RCC ->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC_HSE;
	RCC ->PLLCFGR |=  RCC_PLLCFGR_PLLSRC_HSI;

//	//3.configuramos la frecuencia: Frec(pll) = (16Mhz*(PLLN/PLLM))/PLLP
//	// Escogemos PLLN = 200,PLLM =16, PLLP =2 --> Frec(pll) = 100Mhz
//
	RCC ->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;  //Limpiamos
	RCC ->PLLCFGR |= (200)<<RCC_PLLCFGR_PLLN_Pos; //escribimos 200

	RCC ->PLLCFGR &= ~RCC_PLLCFGR_PLLM;  		 //Limpiamos
	RCC ->PLLCFGR |= (16)<<RCC_PLLCFGR_PLLM_Pos; //escribimos 16

	RCC ->PLLCFGR |= (0b00) << RCC_PLLCFGR_PLLP_Pos;  //Limpiamos (escribimos 2)

	RCC ->PLLCFGR &= ~RCC_PLLCFGR_PLLQ;  	      //Limpiamos
	RCC ->PLLCFGR |= (15<<RCC_PLLCFGR_PLLQ_Pos);  //escribimos 15 (este parametro debe ser menos a 50, escogiendo 15 lo garantizamos

	//4.Configuramos el AHB (debe funcionar a <50 Mhz)
	RCC ->CFGR &=  ~RCC_CFGR_HPRE;      // Limpiamos
	RCC ->CFGR |=   RCC_CFGR_HPRE_DIV4; // Ponemos el prescaler a div 4.


	//5. Encendemos el PLL
	RCC ->CR |= RCC_CR_PLLON;		 // habilitamos el PLL
	RCC->CFGR |= RCC_CFGR_SW_PLL;	 //Ponemos al sistema a funcionar con el PLL
	while(!(RCC->CFGR & RCC_CFGR_SWS_1)){
		__NOP();
	}
	//6. Configuramos el Microcontroller clock output
	//Configuramos el prescaler para poder leerlo con el analizador
	RCC->CFGR &= ~RCC_CFGR_MCO1PRE; //Limpiamos
	RCC->CFGR |= (0b111) << RCC_CFGR_MCO1PRE_Pos; //Congifuramos el prescaler a 5

	//Seleccionamos el pin MCO como salida para el PLL
	RCC->CFGR &= ~RCC_CFGR_MCO1; //Limpiamos
	RCC->CFGR |= RCC_CFGR_MCO1_1; //Escribimos 10
	RCC->CFGR |= RCC_CFGR_MCO1_0; //Escribimos 01



}

//para en un Futuro cambiar la frecuencia a una seleccionada


//void setFrecuency_Mhz(uint8_t frecuency){
////	//3.configuramos la frecuencia: Frec(pll) = (16Mhz*(PLLN/PLLM))/PLLP
////	// Escogemos PLLN PLLM  PLLP
//	RCC ->PLLCFGR &= ~RCC_PLLCFGR_PLLN;  //Limpiamos
//	RCC ->PLLCFGR |= (frecuency*2)<<RCC_PLLCFGR_PLLN_Pos;
//
//	if(frecuency >0 && frecuency <45){
//		//4.Configuramos el AHB
//		RCC ->CFGR &=  ~RCC_CFGR_HPRE;      // Limpiamos el prescaler
//
//	}
//	else if(frecuency >= 45 && frecuency <= 90){
//		//4.Configuramos el AHB
//		RCC ->CFGR &=  ~RCC_CFGR_HPRE;      // Limpiamos
//		RCC ->CFGR |=   RCC_CFGR_HPRE_DIV2; // Ponemos el prescaler a div por 2. (porque el timer funciona a maximo 45 MHz)
//	}
//	else if(frecuency > 90 && frecuency <= 100){
//		//4.Configuramos el AHB
//		RCC ->CFGR &=  ~RCC_CFGR_HPRE;      // Limpiamos
//		RCC ->CFGR |=   RCC_CFGR_HPRE_DIV4; // Ponemos el prescaler a div por 4. (porque el timer funciona a maximo 45 MHz)
//	}
//}
