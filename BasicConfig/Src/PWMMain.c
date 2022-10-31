/*
 * PWMMain.c
 *
 *  Created on: Oct 30, 2022
 *      Author: Camilo
 */


#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "PWMDriver.h"

PWM_Handler_t 		handlerPWM = {0};
GPIO_Handler_t       handlerPIN = {0};
GPIO_Handler_t       handlerBlinky = {0};
BasicTimer_Handler_t handlerTimer2 = {0};


int main(void){
	handlerPIN.pGPIOx                                = GPIOA;
	handlerPIN.GPIO_PinConfig.GPIO_PinNumber         = PIN_0;
	handlerPIN.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerPIN.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerPIN.GPIO_PinConfig.GPIO_PinSpeed	         = GPIO_OSPEED_FAST;
	handlerPIN.GPIO_PinConfig.GPIO_PinPuPdControl	 = GPIO_PUPDR_NOTHING;
	handlerPIN.GPIO_PinConfig.GPIO_PinAltFunMode     = AF1;
    GPIO_Config(&handlerPIN);
//    GPIO_WritePin(&handlerBlinky, SET);

    handlerPWM.ptrTIMx			 = TIM2;
    handlerPWM.config.channel    = PWM_CHANNEL_1;
    handlerPWM.config.periodo    = 150;
    handlerPWM.config.prescaler  = BTIMER_SPEED_1ms;  //250ms
    handlerPWM.config.duttyCicle = 50;
    pwm_Config(&handlerPWM);


    handlerBlinky.pGPIOx                                = GPIOA;
	handlerBlinky.GPIO_PinConfig.GPIO_PinNumber         = PIN_5;
	handlerBlinky.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_OUT;
	handlerBlinky.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerBlinky.GPIO_PinConfig.GPIO_PinSpeed	        = GPIO_OSPEED_FAST;
	handlerBlinky.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
    GPIO_Config(&handlerBlinky);

	handlerTimer2.ptrTIMx = TIM4;
	handlerTimer2.TIMx_Config.TIMx_mode   = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed  = BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period = 2500;  //250ms
	BasicTimer_Config(&handlerTimer2);

	startTimer(&handlerTimer2);
	enableOutput(&handlerPWM);
    startPwmSignal(&handlerPWM);

    while(1){

    }
}

void BasicTimer4_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}
