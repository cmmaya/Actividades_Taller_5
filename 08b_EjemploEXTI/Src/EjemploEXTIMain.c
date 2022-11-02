
#include <stdint.h>
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "ExtiDriver.h"

GPIO_Handler_t handlerUserButton = {0};
GPIO_Handler_t handlerUserLed    = {0};
EXTI_Config_t  handlerIntButton  = {0};

int main(void){
	handlerUserButton.pGPIOx = GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber = PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	handlerUserLed.pGPIOx = GPIOA;
	handlerUserLed.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
	handlerUserLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	handlerUserLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OTYPE_PUSHPULL;
	GPIO_Config(&handlerUserLed);

	handlerIntButton.pGPIOHandler  = &handlerUserButton;
	handlerIntButton.edgetype      = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerIntButton);
	while(1){

	}

	return 0;
}

void callback_exInt13(void){
	GPIO_TooglePin(&handlerUserLed);
}
