
#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"


BasicTimer_Handler_t handlerTimer2 = {0};
GPIO_Handler_t       handlerBlinky = {0};


uint8_t blinkState = 0;


int main(void){

	handlerBlinky.pGPIOx                                = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber         = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed	        = GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
    GPIO_Config(&handlerBlinky);
//    GPIO_WritePin(&handlerBlinky, SET);

	handlerTimer2.ptrTIMx = TIM4;
	handlerTimer2.TIMx_Config.TIMx_mode   = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed  = BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period = 2500;  //250ms

	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);


while(1){
//		__NOP();
	}


return 0;
}

// callback de la interrupcion del Timer2

void BasicTimer4_Callback(void){
	blinkState = !blinkState;
	 GPIO_WritePin(&handlerBlinky, blinkState);
	if(blinkState){
	    GPIO_WritePin(&handlerBlinky, SET);
	}
	else{
	    GPIO_WritePin(&handlerBlinky, RESET);
	}
}

