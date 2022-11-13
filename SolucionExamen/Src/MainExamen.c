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
#include "PwmDriver.h"

//RTC
char *datosTiempo;

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

I2C_Handler_t handlerAccelerometer = { 0 };
I2C_Handler_t handlerLCD = { 0 };

PWM_Handler_t 		handlerPWM1 = {0};
PWM_Handler_t 		handlerPWM2 = {0};
PWM_Handler_t 		handlerPWM3 = {0};

uint8_t counterReception = 0;
bool stringComplete = false;
unsigned int firstParameter;
unsigned int secondParameter;
char cmd[64] = {0};
char userMsg[64] = {0};
uint8_t initdisplay = 0;
int hsvPeriod = 250;

//Defino los macros y las variables auxiliares
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
int16_t PX = 0;
int16_t PY = 0;
int16_t PZ = 0;
uint16_t lapse = 0;
uint8_t SU;
uint8_t ST;
uint8_t MNT;
uint8_t MNU;
uint8_t HU;
uint8_t HT;



//Funciones
void initSystem(void);
void InitLCD();
void delay(uint8_t H);
void writeData(uint8_t cmd);
void writeCmd(uint8_t cmd);
void cursorSet(uint8_t col, uint8_t row);
void sendString(uint8_t datos[]);
uint8_t HsvToRgb(int16_t x, int16_t y);
int16_t getAccel(void);
void parseCommands(char *ptrBufferReception);
void writeString(char *array);
void selectPos(uint8_t col, uint8_t row);

//Funcion Principal
int main(void) {

	rtc_Config();
	initSystem();
	i2c_writeSingleRegister(&handlerAccelerometer, ACCEL_PWR, 0b00001000); //Inicio el Acelerometro



	while (1) {
		datosTiempo = rtc_GetData(); //obtenemos los datos de tiempo


		if (rxData != '\0') {
			//creamos una cadena e identificamos si llega "@" al final
			bufferReception[counterReception] = rxData;
			counterReception++;

			if (rxData == '@') {
				stringComplete = true;

				//agrego linea null para crear string
				bufferReception[counterReception] = '\0';
				counterReception = 0;
			}
			rxData ='\0';

		}
		if(stringComplete){
			parseCommands(bufferReception);
			stringComplete = false;
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

	handlerRgbGround.pGPIOx                             = GPIOC;
	handlerRgbGround.GPIO_PinConfig.GPIO_PinNumber      = PIN_0;
	handlerRgbGround.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerRgbGround.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerRgbGround.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerRgbGround.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerRgbGround.GPIO_PinConfig.GPIO_PinAltFunMode  = AF0;
	GPIO_Config(&handlerRgbGround);
	GPIO_WritePin(&handlerRgbGround, RESET);

	//config los pines del usart
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber  	= PIN_9;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;
	GPIO_Config(&handlerPinTx);


	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber  	= PIN_10;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode    	= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType  	= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed   	= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode  = AF7;
	GPIO_Config(&handlerPinRx);

	handlerCommTerminal.ptrUSARTx = USART1;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_57600;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_9BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_EVEN;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerCommTerminal);

	//Config los timer ///////////
	handlerStateLed.ptrTIMx 						= TIM2;
	handlerStateLed.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerStateLed.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerStateLed.TIMx_Config.TIMx_period 		= 250;	//periodod de 250 ms
	handlerStateLed.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerStateLed);
	startTimer(&handlerStateLed);

	handlerTim4.ptrTIMx 						= TIM4;
	handlerTim4.TIMx_Config.TIMx_mode 			= BTIMER_MODE_UP;
	handlerTim4.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerTim4.TIMx_Config.TIMx_period 		= hsvPeriod;	//periodod de 250 ms
	handlerTim4.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerTim4);
	startTimer(&handlerTim4);

	//I2C config para el accel///////////
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


	//I2C config para el LCD////////
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

	enableOutput(&handlerPWM1);
    startPwmSignal(&handlerPWM1);

	enableOutput(&handlerPWM2);
    startPwmSignal(&handlerPWM2);

	enableOutput(&handlerPWM3);
    startPwmSignal(&handlerPWM3);

}

void usart1Rx_Callback(void){
	rxData = getRxData();
	}

void BasicTimer2_Callback(void){
	delayFlag +=1;
}

void BasicTimer4_Callback(void){
	GPIOxTooglePin(&handlerRgbGround);
	GPIOxTooglePin(&handlerLedOK);
	tim4Flag = 1;
}

//Usando el Timer 2, creamos un delay. 1 ciclo es 1/64 segundo
void delay(uint8_t H){
	delayFlag = 0;
	while(delayFlag<H){
		__NOP();
	}
}

//funcion para escribir comandos en 4 bit (rs=0)
void writeCmd(uint8_t cmd){
	uint8_t cmdMS = cmd & 0xf0;
	uint8_t cmdLS = (cmd<<4) & 0xf0;

	uint8_t dato1 = cmdMS | 0x0C; //rs =0 rw=1 en=1
	uint8_t dato2 = cmdMS | 0x08; //rs =0 rw=1 en=0
	uint8_t dato3 = cmdLS | 0x0C;
	uint8_t dato4 = cmdLS | 0x08;

	lcd_writeSingleRegister(&handlerLCD, dato1);
	lcd_writeSingleRegister(&handlerLCD, dato2);
	lcd_writeSingleRegister(&handlerLCD, dato3);
	lcd_writeSingleRegister(&handlerLCD, dato4);
}
//Funcion para andar datos a la pantalla en 4 bit (rs=1)
void writeData(uint8_t cmd){
	uint8_t cmdMS = cmd & 0xf0;
	uint8_t cmdLS = (cmd<<4) & 0xf0;

	uint8_t dato1 = cmdMS | 0x0D; //rs =1 rw=1 en=1
	uint8_t dato2 = cmdMS | 0x09; //rs =1 rw=1 en=0
	uint8_t dato3 = cmdLS | 0x0D;
	uint8_t dato4 = cmdLS | 0x09;

	lcd_writeSingleRegister(&handlerLCD, dato1);
	lcd_writeSingleRegister(&handlerLCD, dato2);
	lcd_writeSingleRegister(&handlerLCD, dato3);
	lcd_writeSingleRegister(&handlerLCD, dato4);
}

//Secuencia de inicializacion
void InitLCD(){
	delay(64);
	writeCmd(0x30);
	delay(16);
	writeCmd(0x30);
	delay(16);
	writeCmd(0x32);
	writeCmd(0x28);
	writeCmd(0x0C);
	writeCmd(0x01);
	writeCmd(0x06);
}

//Para mandar una cadena de ascii
void writeString(char *array){
	uint8_t i = 0;
	while (!(array[i] == '\0')) {
		writeData(array[i]);
		i++;
	}
}

//Seleccionamos la posicion para mandar los datos
void selectPos(uint8_t col, uint8_t row){
	uint8_t i = 0;

	switch(row){
	case 0:{
		writeCmd(0x80 + col);
		break;
	}
	case 1:{
		writeCmd(0xC0 + col);
		break;
	}
	case 2:{
		writeCmd(0x80 + col);
		while(i<20){
			writeData(' ');
			i++;
		}
		break;
	}
	case 3:{
		writeCmd(0xC0 + col);
		while(i<20){
			writeData(' ');
			i++;
		}
		break;
	}

	default:{
		writeCmd(0x80 + col);
	}
	}
}

//Conversor de HSV a RGB, utilizando el angulo que me dan las 2 componentes
//del acelerometro
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

//Me entrega los valores de RGB
	return R;
	return G;
	return B;

}

//obtenemos la aceleracion de los registros del acelerometro
int16_t getAccel(void){
	Z1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Z1);
	Z0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Z0);
	AccelZ = Z0 <<8 | Z1;
	//Le definimos un limite pata que no tome los valores criticos
	if(AccelZ > -6400){
		Y1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y1);
		Y0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_Y0);
		AccelY = Y0 <<8 | Y1;
		X1 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X1);
		X0 = i2c_readSingleRegister(&handlerAccelerometer, ACCEL_X0);
		AccelX = X0 <<8 | X1;
	}
	else{
		__NOP();
	}
	delay(16);

	//Devolvemos los valores de X,Y y Z
	return AccelX;
	return AccelY;
	return AccelZ;

}

//Funcion para utilizar los comandos
void parseCommands(char *ptrBufferReception) {


	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter,&secondParameter, userMsg);
	if (strcmp(cmd, "help") == 0) {
		writeMsg(&handlerCommTerminal, "Help Menu CMSs: \n");
		writeMsg(&handlerCommTerminal, "1) help             --Print this menu \n");
		writeMsg(&handlerCommTerminal, "2) getaccel         --Obtain acceleration, if usermsg x= AccelX, if usermsg y = AccelY ...etc. \n");
		writeMsg(&handlerCommTerminal, "3) initdisplay      --initialize Display\n");
		writeMsg(&handlerCommTerminal, "4) counttime        --Set time to 0, press p to pause, after paused, time displayed in LCD \n");
		writeMsg(&handlerCommTerminal, "5) setime           --Set time cmd_#hour_#min, press p to pause. Time is displayed in the LCD \n");
		writeMsg(&handlerCommTerminal, "6) starthsv         --Start led hsv behaviour, press p to pause  \n");
		writeMsg(&handlerCommTerminal, "7) setperiod        --SET led hsv blink period, pause the hsv before changing it \n");
		writeMsg(&handlerCommTerminal, "8) writestring      --writes UserMsg in the LCD row 2, initdisplay before trying \n");
		writeMsg(&handlerCommTerminal, "9) animation        --START an animation in the LCD, initdisplay before trying, try firstparam = 1 or 2 \n");
		writeMsg(&handlerCommTerminal, "10) selectpos       --PRINTS an x in the selected pos, firstparam:col and secondparam:row \n");
		writeMsg(&handlerCommTerminal, "11) cleardisplay    --clears display \n");



	}
	else if (strcmp(cmd, "dummy") == 0) {
		writeMsg(&handlerCommTerminal, "CMD: dummy \n");
	}
	//0. Gets acceleratopm and Displays in LCD
	else if (strcmp(cmd, "getaccel") == 0) {
		if(initdisplay){
		//1. obtenemos la aceleracion y le sacamos el porcentaje
		getAccel();
		PX = AccelX*100/32768;
		PY = AccelY*100/32768;
		PZ = AccelZ*100/32768;

		writeMsg(&handlerCommTerminal, "CMD: get Acceleration \n");
		writeCmd(0x01);
		delay(16);
		selectPos(1, 5);

		//desplegamos en el LCD cada caso
		if (strcmp(userMsg, "x") == 0){
			sprintf(bufferData, "AccelX= %d", PX);
			writeString(bufferData);
			writeData(0x25);
		}

		else if (strcmp(userMsg, "y") == 0){
			sprintf(bufferData, "AccelY= %d", PY);
			writeString(bufferData);
			writeData(0x25);
		}

		else if (strcmp(userMsg, "z") == 0){
			sprintf(bufferData, "AccelZ= %d", PZ);
			writeString(bufferData);
			writeData(0x25);
		}

		else {
			writeMsg(&handlerCommTerminal, "introduce a valid component \n");
		}
		}
		else{
			writeMsg(&handlerCommTerminal, "initdisplay please \n");

		}

	}
	else if(strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerCommTerminal, "CMD: user message \n");
		writeMsg(&handlerCommTerminal, userMsg);
		writeMsg(&handlerCommTerminal, "\n");
	}
	//2. Inicializamos el display
	else if(strcmp(cmd, "initdisplay") == 0){
		writeMsg(&handlerCommTerminal, "CMD: init display \n");
		InitLCD();  //Inicio el LCD

		initdisplay = 1;
	}
	//3. Contamos el tiempo
	else if(strcmp(cmd, "counttime") == 0){
		writeMsg(&handlerCommTerminal, "CMD: start RTC timer \n");
		writeCmd(0x01);
		delay(16);
		//Setiamos el display en ceros
		rtc_Config();
		rtc_SetTime(0, 0, 0);
		delay(16);
		//hasta que no hunda "p" mandamos al Coolterm la hora
		while (!(rxData == 'p')) {
			datosTiempo = rtc_GetData(); //obtenemos los datos de tiempo
			SU  = datosTiempo[0];
			ST  = datosTiempo[1];
			MNU = datosTiempo[2];
			MNT = datosTiempo[3];
			HU  = datosTiempo[4];
			HT  = datosTiempo[5];
			sprintf(bufferData, "time set to %d%d:%d%d:%d%d \n", HT, HU, MNT, MNU, ST, SU);
			writeMsg(&handlerCommTerminal, bufferData);
			delay(64);

		}
		//cuando salga del while calculamos el tiempo transcurrido y lo desplegamos en pantalla
		lapse = 60*(10*MNT+MNU)+(10*ST+SU);

		sprintf(bufferData, "seconds = %d", (int) lapse);
		writeString(bufferData);


	}
	//4. desplegamos el timepo en pantalla
	else if(strcmp(cmd, "settime") == 0){
		if(initdisplay){
		writeMsg(&handlerCommTerminal, "CMD: set RTC timer \n");
		writeMsg(&handlerCommTerminal, "press p to pause  \n");

		//Configuramos el tiempo
		rtc_Config();
		rtc_SetTime(firstParameter, secondParameter, 0);

		//hasta que no se hunda "p" mandamos al LCD los datos de la hora
		while (!(rxData == 'p')) {
			datosTiempo = rtc_GetData(); //obtenemos los datos de tiempo
				uint8_t SU  = datosTiempo[0] + 48;
				uint8_t ST  = datosTiempo[1] + 48;
				uint8_t MNU = datosTiempo[2] + 48;
				uint8_t MNT = datosTiempo[3] + 48;
				uint8_t HU  = datosTiempo[4] + 48;
				uint8_t HT  = datosTiempo[5] + 48;
				writeCmd(0x85);
				writeData(HT);
				writeData(HU);
				writeData(58);
				writeData(MNT);
				writeData(MNU);
				writeData(58);
				writeData(ST);
				writeData(SU);
		}
		writeMsg(&handlerCommTerminal, "stop  \n");

		}
		else{
			writeMsg(&handlerCommTerminal, "init display please \n");

		}

	}
	//5. definimos el periodo del LED
	else if(strcmp(cmd, "setperiod") == 0){
		writeMsg(&handlerCommTerminal, "CMD: set period to the LED  \n");

		handlerTim4.TIMx_Config.TIMx_period  = firstParameter;
		BasicTimer_Config(&handlerTim4);

	}
	//6. Empezamos el HSV
	else if(strcmp(cmd, "starthsv") == 0){
		writeMsg(&handlerCommTerminal, "CMD: start HSV  \n");
		// Obtenemos continuamente los valoes de RGB y actualizamos los PWM
		while (!(rxData == 'p')) {
			getAccel();
			HsvToRgb(AccelX,AccelY);
			updateDuttyCycle(&handlerPWM1, B* 0.588);
			updateDuttyCycle(&handlerPWM2, G* 0.588);
			updateDuttyCycle(&handlerPWM3, R* 0.588);
			delay(5);
		}
		rxData=0;
		writeMsg(&handlerCommTerminal, "stop \n");

	}
	//7. Escribimos varios ascii
	else if(strcmp(cmd, "writestring") == 0){
		if(initdisplay){

		writeMsg(&handlerCommTerminal, "CMD: Write String in LCD  \n");

		writeString(userMsg);
		}
		else{
			writeMsg(&handlerCommTerminal, "init display please \n");
	}
	}
	//8. Desplegamos 2 animaciones
	else if(strcmp(cmd, "animation") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Animation  \n");
		position = 131;
		writeCmd(0x01);
		delay(16);
		//Caso 1: Vamos cambindo la posiicion del UserMSg, verticalmente
		switch (firstParameter) {
		case 1: {
			while(!(rxData=='p')){
			selectPos(0, 0);
			writeString(userMsg);
			delay(25);

			selectPos(0, 1);
			writeString(userMsg);
			delay(25);

			selectPos(0, 2);
			writeString(userMsg);
			delay(25);

			selectPos(0, 3);
			writeString(userMsg);
			delay(25);

			selectPos(12, 0);
			writeString(userMsg);
			delay(25);

			selectPos(12, 1);
			writeString(userMsg);
			delay(25);

			selectPos(12, 2);
			writeString(userMsg);
			delay(25);

			selectPos(12, 3);
			writeString(userMsg);
			delay(25);

			writeCmd(0x01);
			delay(64);
			}
			writeMsg(&handlerCommTerminal, "stop  \n");
			rxData = 0;
			break;
			}
		//Caso 2: Vamos cambindo la posiicion del UserMSg, verticalmente

		case 2:{
			uint8_t i = 0;
			for(i=0; i<40; i++){
				writeCmd(0x01);
				delay(2);
				selectPos(i, 0);
				writeString(userMsg);
				delay(64);

			}
			break;
		}
		default:{
			break;

		}
		}
		}


	//9. Select a position in LCD
	else if(strcmp(cmd, "selectpos") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Select Position  \n");
		//selects position
		selectPos(firstParameter, secondParameter);
		//writes an X
		writeData('x');

	}
	//10. Clears Display
	else if(strcmp(cmd, "cleardisplay") == 0){
		writeMsg(&handlerCommTerminal, "CMD: Clear Display  \n");
		writeCmd(0x01);

	}


	}


//Revisión Examen.
//- El equipo no funcionó inicialmente...
//- Los datos del acelerometro no funcionan correctamente (ejeY y ejeZ)
//- La lista de comando tiene problemas...
//- El Led RGB funciona con ciertos problemas con respecto al angulo del Accel.
//- Agregar las referencias de donde se sacó el código.
//- Hace falta poner mas comentarios en el código
//
//(15+25+12+3+10+20)*5/100
//
//Calificación = 4.25

