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

//Defino variables
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerPinRx = { 0 };
GPIO_Handler_t handlerI2cSDA = { 0 };
GPIO_Handler_t handlerI2cSCL = { 0 };
GPIO_Handler_t LCDI2cSDA = { 0 };
GPIO_Handler_t LCDI2cSCL = { 0 };
GPIO_Handler_t handlerRgbGround = { 0 };
GPIO_Handler_t       handlerAF1 = {0};
GPIO_Handler_t       handlerAF2 = {0};
GPIO_Handler_t       handlerAF3 = {0};

BasicTimer_Handler_t handlerStateLed = { 0 };
BasicTimer_Handler_t handlerTim4 = {0};
int delayFlag = 0;
uint8_t tim4Flag = 0;

USART_Handler_t handlerCommTerminal = { 0 };
uint8_t rxData = 0;
char greetingMsg[] = "Taller V Rocks!\n";
char bufferData[64] = "esto es una prueba";
char bufferReception[64] = {0};


uint8_t counterReception = 0;
bool stringComplete = false;
unsigned int firstParameter;
unsigned int secondParameter;
char cmd[64] = {0};
char userMsg[64] = {0};
uint8_t initdisplay = 0;
int hsvPeriod = 250;

I2C_Handler_t handlerAccelerometer = { 0 };
I2C_Handler_t handlerLCD = { 0 };

PWM_Handler_t 		handlerPWM1 = {0};
PWM_Handler_t 		handlerPWM2 = {0};
PWM_Handler_t 		handlerPWM3 = {0};



#define PI 3.141592654
#define ACCEL_ADDRESS 0b01010011;
#define ACCEL_PWR 45
#define ACCEL_X0 50
#define ACCEL_X1 51
#define ACCEL_Y0 52
#define ACCEL_Y1 53
#define ACCEL_Z0 54
#define ACCEL_Z1 55
#define LCD_ADDRESS 0x27;
uint8_t R;
uint8_t G;
uint8_t B;
uint8_t X1;
uint8_t X0;
uint8_t Y1;
uint8_t Y0;
uint8_t Z0;
uint8_t Z1;
int16_t AccelX;
int16_t AccelY;
int16_t AccelZ;
uint8_t position = 0;

PWM_Handler_t 		handlerPWM1 = {0};
PWM_Handler_t 		handlerPWM2 = {0};
PWM_Handler_t 		handlerPWM3 = {0};
GPIO_Handler_t       handlerAF1 = {0};
GPIO_Handler_t       handlerAF2 = {0};
GPIO_Handler_t       handlerAF3 = {0};
GPIO_Handler_t       handlerBlinky = {0};
BasicTimer_Handler_t handlerTimer2 = {0};

void initSystem(void);
uint16_t HsvToRgb(int x, int y);

int main(void){
	initSystem();
	enableOutput(&handlerPWM1);
    startPwmSignal(&handlerPWM1);

	enableOutput(&handlerPWM2);
    startPwmSignal(&handlerPWM2);

	enableOutput(&handlerPWM3);
    startPwmSignal(&handlerPWM3);


    while(1){
		if (rxData != '\0') {

			if (rxData == 'm') {
				writeMsg(&handlerCommTerminal, greetingMsg);
				rxData = '\0';
			}

			else if (rxData == 'x') {
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else {
				rxData = '\0';
			}

		}


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

	handlerTimer2.ptrTIMx = TIM4;
	handlerTimer2.TIMx_Config.TIMx_mode   = BTIMER_MODE_UP;
	handlerTimer2.TIMx_Config.TIMx_speed  = BTIMER_SPEED_100us;
	handlerTimer2.TIMx_Config.TIMx_period = 2500;  //250ms
	BasicTimer_Config(&handlerTimer2);
	startTimer(&handlerTimer2);

	//// Configuro los pines para los PWM CH1
	handlerAF1.pGPIOx                                = GPIOA;
	handlerAF1.GPIO_PinConfig.GPIO_PinNumber         = PIN_0;
	handlerAF1.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerAF1.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerAF1.GPIO_PinConfig.GPIO_PinSpeed	         = GPIO_OSPEED_FAST;
	handlerAF1.GPIO_PinConfig.GPIO_PinPuPdControl	 = GPIO_PUPDR_NOTHING;
	handlerAF1.GPIO_PinConfig.GPIO_PinAltFunMode     = AF1;
    GPIO_Config(&handlerAF1);


    //CH2
	handlerAF2.pGPIOx                                = GPIOA;
	handlerAF2.GPIO_PinConfig.GPIO_PinNumber         = PIN_1;
	handlerAF2.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerAF2.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerAF2.GPIO_PinConfig.GPIO_PinSpeed	         = GPIO_OSPEED_FAST;
	handlerAF2.GPIO_PinConfig.GPIO_PinPuPdControl	 = GPIO_PUPDR_NOTHING;
	handlerAF2.GPIO_PinConfig.GPIO_PinAltFunMode     = AF1;
    GPIO_Config(&handlerAF2);

    //CH3
	handlerAF3.pGPIOx                                = GPIOB;
	handlerAF3.GPIO_PinConfig.GPIO_PinNumber         = PIN_10;
	handlerAF3.GPIO_PinConfig.GPIO_PinMode           = GPIO_MODE_ALTFN;
	handlerAF3.GPIO_PinConfig.GPIO_PinOPType         = GPIO_OTYPE_PUSHPULL;
	handlerAF3.GPIO_PinConfig.GPIO_PinSpeed	         = GPIO_OSPEED_FAST;
	handlerAF3.GPIO_PinConfig.GPIO_PinPuPdControl	 = GPIO_PUPDR_NOTHING;
	handlerAF3.GPIO_PinConfig.GPIO_PinAltFunMode     = AF1;
    GPIO_Config(&handlerAF3);



	//// Configuro los pines para los PWM
    handlerPWM1.ptrTIMx			  = TIM2; //AZUL
    handlerPWM1.config.channel    = PWM_CHANNEL_1;
    handlerPWM1.config.periodo    = 1500;
    handlerPWM1.config.prescaler  = BTIMER_SPEED_100us;  //250ms
    handlerPWM1.config.duttyClice = 3;
    pwm_Config(&handlerPWM1);

    handlerPWM2.ptrTIMx			  = TIM2;//VERDE
    handlerPWM2.config.channel    = PWM_CHANNEL_2;
    handlerPWM2.config.periodo    = 1500;
    handlerPWM2.config.prescaler  = BTIMER_SPEED_100us;  //250ms
    handlerPWM2.config.duttyCicle = 15;
    pwm_Config(&handlerPWM2);

    handlerPWM3.ptrTIMx			  = TIM2;//ROJO
    handlerPWM3.config.channel    = PWM_CHANNEL_3;
    handlerPWM3.config.periodo    = 1500;
    handlerPWM3.config.prescaler  = BTIMER_SPEED_100us;  //250ms
    handlerPWM3.config.duttyCicle = 0;
    pwm_Config(&handlerPWM3);

}
void BasicTimer4_Callback(void){
	GPIOxTooglePin(&handlerBlinky);
}

uint16_t HsvToRgb(int x, int y){
	uint16_t thetha = 0;
	uint16_t R = 0;
	uint16_t G = 0;
	uint16_t B = 0;

	if(x>0 && y>0){
		thetha = atan(y/x);
	}
	else if(x<0 && y>0){
		thetha = PI + atan(y/x);
	}
	else if(x<0 && y<0){
		thetha = PI + atan(y/x);
	}
	else if(x>0 && y<0){
		thetha = 2*PI + atan(y/x);
	}

	int16_t M = 255*thetha;
	int16_t S = 32767/(sqrt(x*x+y*y));
	int16_t m = M*(1-S);
	int16_t z = (M-m)*(1 - abs((thetha/60)%2 - 1));


	if(thetha >= 0 && thetha <= 60){
	 R = M;
	 G = z+m;
	 B = m;
	}
	if(thetha >= 60 && thetha <= 120){
	 R = z+m;
	 G = M;
	 B = m;
	}
	else if(thetha >= 120 && thetha <= 180){
	 R = m;
	 G = M;
	 B = z+m;
	}
	else if(thetha >= 180 && thetha <= 240){
	 R = m;
	 G = z+m;
	 B = M;
	}
	else if(thetha >= 240 && thetha <= 300){
	 R = z+m;
	 G = m;
	 B = M;
	}
	else if(thetha >= 300 && thetha <= 360){
	 R = M;
	 G = m;
	 B = z+m;
	}
	return R;
	return G;
	return B;

}
