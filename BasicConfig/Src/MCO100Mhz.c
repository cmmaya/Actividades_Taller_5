/*
 * MCO100Mhz.c
 */



#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "InternalFrecuency.h"


GPIO_Handler_t handlerBlinky = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerPinRx = { 0 };
GPIO_Handler_t handlerMCO1 = { 0 };


void initSystem(void);
int main(void){
	initSystem();
	internalFrecuency_config();
	while(1){
		__NOP();
	}
}

void initSystem(void){
    handlerBlinky.pGPIOx                                = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber         = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed	        = GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
    GPIO_Config(&handlerBlinky);

    handlerMCO1.pGPIOx                                = GPIOA;
    handlerMCO1.GPIO_PinConfig.GPIO_PinNumber         = PIN_8;
    handlerMCO1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
    handlerMCO1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
    handlerMCO1.GPIO_PinConfig.GPIO_PinSpeed	      = GPIO_OSPEED_FAST;
    handlerMCO1.GPIO_PinConfig.GPIO_PinPuPdControl	  = GPIO_PUPDR_NOTHING;
    handlerMCO1.GPIO_PinConfig.GPIO_PinAltFunMode	  = AF0;

    GPIO_Config(&handlerMCO1);


}









