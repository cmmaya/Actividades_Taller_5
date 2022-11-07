#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"
#include "RTCDriver.h"
#include "PWMDriver.h"

//RTC
char *datosTiempo;

//Defino variables
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerPinRx = { 0 };

BasicTimer_Handler_t handlerStateLed = { 0 };
int delayFlag = 0;

USART_Handler_t handlerCommTerminal = { 0 };
uint8_t rxData = 0;
char bufferData[64] = "esto es una prueba";
char greetingMsg[] = "Taller V Rocks!\n";

GPIO_Handler_t handlerI2cSDA = { 0 };
GPIO_Handler_t handlerI2cSCL = { 0 };
I2C_Handler_t handlerAccelerometer = { 0 };
uint8_t i2cBuffer = 0;

GPIO_Handler_t LCDI2cSDA = { 0 };
GPIO_Handler_t LCDI2cSCL = { 0 };
I2C_Handler_t handlerLCD = { 0 };

PWM_Handler_t 		handlerPWM1 = {0};
PWM_Handler_t 		handlerPWM2 = {0};
PWM_Handler_t 		handlerPWM3 = {0};
GPIO_Handler_t       handlerAF1 = {0};
GPIO_Handler_t       handlerAF2 = {0};
GPIO_Handler_t       handlerAF3 = {0};
GPIO_Handler_t       handlerBlinky = {0};
BasicTimer_Handler_t handlerTimer2 = {0};

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
void initSystem(void);
void InitLCD();
void delay(uint8_t H);
void writeData(uint8_t cmd);
void writeCmd(uint8_t cmd);
void cursorSet(uint8_t col, uint8_t row);
void sendString(uint8_t datos[]);
uint8_t HsvToRgb(int16_t x, int16_t y);
//Funcion Principal
int main(void) {
	initSystem();
	i2c_writeSingleRegister(&handlerAccelerometer, ACCEL_PWR, 0b00001000); //Inicio el Acelerometro4
	//Inicio los PWM
	enableOutput(&handlerPWM1);
    startPwmSignal(&handlerPWM1);

	enableOutput(&handlerPWM2);
    startPwmSignal(&handlerPWM2);

	enableOutput(&handlerPWM3);
    startPwmSignal(&handlerPWM3);

	InitLCD();  //Inicio el LCD
	rtc_Config(); //Inicio el RTC


	while (1) {
		datosTiempo = rtc_GetData(); //obtenemos los datos de tiempo

		if (rxData != '\0') {

			if (rxData == 'm') {
				writeMsg(&handlerCommTerminal, greetingMsg);
				rxData = '\0';
			}

			else if (rxData == 'x') {

				uint8_t X1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X1);
				uint8_t X0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X0);
				int16_t AccelX = X0 <<8 | X1;
				sprintf(bufferData, "dataX = %d \n", (int) AccelX);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'y') {

				uint8_t Y1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y1);
				uint8_t Y0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y0);
				int16_t AccelY = Y0 <<8 | Y1;
				sprintf(bufferData, "dataY = %d \n", (int) AccelY);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'z') {

				uint8_t Z1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Z1);
				uint8_t Z0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Z0);
				int16_t AccelZ = Z0 <<8 | Z1;
				sprintf(bufferData, "dataZ = %d \n", (int) AccelZ);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'p') {
				rtc_Config();
				rtc_SetTime(22, 59, 40);
				rxData = '\0';
			}
			else if (rxData == 'q') {
				uint8_t SU  = datosTiempo[0] + 48;
				uint8_t ST  = datosTiempo[1] + 48;
				uint8_t MNU = datosTiempo[2] + 48;
				uint8_t MNT = datosTiempo[3] + 48;
				uint8_t HU  = datosTiempo[4] + 48;
				uint8_t HT  = datosTiempo[5] + 48;
				writeCmd(0x85);
				writeData(HT);	delay(1);
				writeData(HU); delay(1);
				writeData(58); delay(1);
				writeData(MNT); delay(1);
				writeData(MNU); delay(1);
				writeData(58); delay(1);
				writeData(ST); delay(1);
				writeData(SU); delay(1);

//				writeCmd(0xC5);
//
//				writeData('R');
//				writeData('I');
//				writeData('L');
//				writeData('E');
//				writeData('Y');

				rxData = '\0';
			}
			else if (rxData == 'g') {

				uint8_t X1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X1);
				uint8_t X0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X0);
				int16_t AccelX = X0 <<8 | X1;
				uint8_t Y1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y1);
				uint8_t Y0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y0);
				int16_t AccelY = Y0 <<8 | Y1;
				HsvToRgb(AccelX, AccelY);

				sprintf(bufferData, "dataZ = %d %d %d \n", (int) R, (int) G, (int) B);
				rxData = '\0';
				updateDuttyCycle(&handlerPWM1, B* 0.588);
				updateDuttyCycle(&handlerPWM2, G* 0.588);
				updateDuttyCycle(&handlerPWM3, R* 0.588);
			}
			else {
				rxData = '\0';
			}

		}
	}
}


void initSystem(void){

	//Pin que controla el led de la board
	handlerLedOK.pGPIOx 							= GPIOA;
	handlerLedOK.GPIO_PinConfig.GPIO_PinNumber  	= PIN_5;
	handlerLedOK.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_OUT;
	handlerLedOK.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerLedOK.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerLedOK.GPIO_PinConfig.GPIO_PinSpeed  		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerLedOK);

	//config los pines del usart
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber  	= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;
	GPIO_Config(&handlerPinTx);


	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber  	= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;
	GPIO_Config(&handlerPinRx);

	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_9600;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerCommTerminal);

	//Config el Timer
	handlerStateLed.ptrTIMx 						= TIM2;
	handlerStateLed.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerStateLed.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerStateLed.TIMx_Config.TIMx_period 		= 250;	//periodod de 250 ms
	handlerStateLed.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerStateLed);
	startTimer(&handlerStateLed);

	//I2C config para el accel
	handlerI2cSCL.pGPIOx                            = GPIOB;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinNumber     = PIN_6;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_PULLUP;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSCL);

	handlerI2cSDA.pGPIOx                            = GPIOB;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinNumber     = PIN_7;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_OPENDRAIN;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_PULLUP;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&handlerI2cSDA);

	handlerAccelerometer.ptrI2Cx = I2C1;
	handlerAccelerometer.modeI2C = I2C_MODE_FM;
	handlerAccelerometer.slaveAddress = ACCEL_ADDRESS;
	i2c_config(&handlerAccelerometer);


	//I2C config para el LCD
	LCDI2cSCL.pGPIOx                            = GPIOA;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinNumber     = PIN_8;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_OPENDRAIN;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_PULLUP;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&LCDI2cSCL);

	LCDI2cSDA.pGPIOx                            = GPIOC;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinNumber     = PIN_9;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_OPENDRAIN;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_PULLUP;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&LCDI2cSDA);

	handlerLCD.ptrI2Cx = I2C3;
	handlerLCD.modeI2C = I2C_MODE_SM;
	handlerLCD.slaveAddress = LCD_ADDRESS;
	i2c_config(&handlerLCD);

	//configuro el PWM///////////////////////////////////
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



	//// Configuro los timer para los PWM
    handlerPWM1.ptrTIMx			  = TIM2; //AZUL
    handlerPWM1.config.channel    = PWM_CHANNEL_1;
    handlerPWM1.config.periodo    = 150;
    handlerPWM1.config.prescaler  = BTIMER_SPEED_100us;  //250ms
    handlerPWM1.config.duttyCicle = 150;
    pwm_Config(&handlerPWM1);

    handlerPWM2.ptrTIMx			  = TIM2;//VERDE
    handlerPWM2.config.channel    = PWM_CHANNEL_2;
    handlerPWM2.config.periodo    = 150;
    handlerPWM2.config.prescaler  = BTIMER_SPEED_100us;  //250ms
    handlerPWM2.config.duttyCicle = 150;
    pwm_Config(&handlerPWM2);

    handlerPWM3.ptrTIMx			  = TIM2;//ROJO
    handlerPWM3.config.channel    = PWM_CHANNEL_3;
    handlerPWM3.config.periodo    = 150;
    handlerPWM3.config.prescaler  = BTIMER_SPEED_100us;  //250ms
    handlerPWM3.config.duttyCicle = 0;
    pwm_Config(&handlerPWM3);
}

void usart2Rx_Callback(void){
	rxData = getRxData();
	}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLedOK);
	delayFlag +=1;
}

void delay(uint8_t H){
	delayFlag = 0;
	while(delayFlag<H){
		__NOP();
	}
}

void writeCmd(uint8_t cmd){

	uint8_t cmdMS = cmd & 0xf0;
	uint8_t cmdLS = (cmd<<4) & 0xf0;

	uint8_t dato1 = cmdMS | 0x04;
	uint8_t dato2 = cmdMS | 0x00;
	uint8_t dato3 = cmdLS | 0x04;
	uint8_t dato4 = cmdLS | 0x00;

	lcd_writeSingleRegister(&handlerLCD, dato1);
	lcd_writeSingleRegister(&handlerLCD, dato2);
	lcd_writeSingleRegister(&handlerLCD, dato3);
	lcd_writeSingleRegister(&handlerLCD, dato4);
}

void writeData(uint8_t cmd){

	uint8_t cmdMS = cmd & 0xf0;
	uint8_t cmdLS = (cmd<<4) & 0xf0;

	uint8_t dato1 = cmdMS | 0x0D; //
	uint8_t dato2 = cmdMS | 0x09;
	uint8_t dato3 = cmdLS | 0x0D;
	uint8_t dato4 = cmdLS | 0x09;

	lcd_writeSingleRegister(&handlerLCD, dato1);
	lcd_writeSingleRegister(&handlerLCD, dato2);
	lcd_writeSingleRegister(&handlerLCD, dato3);
	lcd_writeSingleRegister(&handlerLCD, dato4);
}

void InitLCD(){
	//trabajamos con 8 bits
	lcd_writeSingleRegister(&handlerLCD, 0x30);
	delay(1);
	lcd_writeSingleRegister(&handlerLCD, 0x30);
	delay(1);
	lcd_writeSingleRegister(&handlerLCD, 0x30);
	delay(1);
	lcd_writeSingleRegister(&handlerLCD, 0x20);//4bit mode

	//trabajamos con 4 bits
	delay(1);
	writeCmd(0x20);//Function Set- 4bit - 2line - 5x8
	delay(1);
	writeCmd(0x08);//D=0, C=, D=0   Display off/ data in DDRAM
	delay(1);
	writeCmd(0x01);//Clear Display
	delay(1);
	writeCmd(0x06);//Entry mode	increment cursor, no shift
	delay(1);
	writeCmd(0x0C);//Display on
}


void sendString(uint8_t datos[]){
	uint8_t i=0;
	while(datos[i] != 0){
		writeData(datos[i]);
		i +=1;
	}
}

uint8_t HsvToRgb(int16_t x, int16_t y){
	float thetha = 0;

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
	thetha = (thetha*180)/PI;

	int8_t M = 140;
	float S = sqrt(x*x+y*y)/32767;
	int8_t m = M*(1-S);
	int8_t z = (M-m)*(1 - abs(((int)thetha/60)%2 - 1));


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
