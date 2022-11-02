#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "I2CDriver.h"

//Defino variables
GPIO_Handler_t handlerLedOK = { 0 };
GPIO_Handler_t handlerPinTx = { 0 };
GPIO_Handler_t handlerPinRx = { 0 };

BasicTimer_Handler_t handlerStateLed = { 0 };

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

#define ACCEL_ADDRESS 0b01010011;
#define ACCEL_PWR 45
#define ACCEL_X0 50
#define ACCEL_X1 51
#define ACCEL_Y0 52
#define ACCEL_Y1 53
#define ACCEL_Z0 54
#define ACCEL_Z1 55

#define LCD_ADDRESS 0b01010011;



#define PWR_MGMT_1 107
#define WHO_AM_I 0

//Definicion de funciones
void initSystem(void);

//Funcion Principal
int main(void) {
	initSystem();
	i2c_writeSingleRegister(&handlerAccelerometer, ACCEL_PWR, 0b00001000);
	while (1) {


		if (rxData != '\0') {

			if (rxData == 'm') {
				writeMsg(&handlerCommTerminal, greetingMsg);
				rxData = '\0';
			}

			else if (rxData == 'x') {

				uint8_t X1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X1);
				uint8_t X0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X0);
				uint16_t AccelX = X0 <<8 | X1;
				sprintf(bufferData, "dataX = %d \n", (int) AccelX);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'y') {

				uint8_t Y1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y1);
				uint8_t Y0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y0);
				uint16_t AccelY = Y0 <<8 | Y1;
				sprintf(bufferData, "dataY = %d \n", (int) AccelY);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}

			else if (rxData == 'z') {

				uint8_t Z1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Z1);
				uint8_t Z0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Z0);
				uint16_t AccelZ = Z0 <<8 | Z1;
				sprintf(bufferData, "dataZ = %d \n", (int) AccelZ);
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
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_115200;
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
	LCDI2cSCL.pGPIOx                            = GPIOB;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinNumber     = PIN_10;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_OPENDRAIN;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_PULLUP;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	LCDI2cSCL.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&LCDI2cSCL);

	LCDI2cSDA.pGPIOx                            = GPIOB;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinNumber     = PIN_11;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_OPENDRAIN;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinPuPdControl= GPIO_PUPDR_PULLUP;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	LCDI2cSDA.GPIO_PinConfig.GPIO_PinAltFunMode = AF4;
	GPIO_Config(&LCDI2cSDA);

	handlerLCD.ptrI2Cx = I2C2;
	handlerLCD.modeI2C = I2C_MODE_SM;
	handlerLCD.slaveAddress = LCD_ADDRESS;
	i2c_config(&handlerLCD);
}

void usart2Rx_Callback(void){
	rxData = getRxData();
	}

void BasicTimer2_Callback(void){
	GPIOxTooglePin(&handlerLedOK);
}
