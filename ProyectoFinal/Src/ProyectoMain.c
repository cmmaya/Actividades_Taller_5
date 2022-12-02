/*
 * CapturaFrecMain.c
 */


#include "stm32f4xx.h"

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "ExtiDriver.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "PwmDriver.h"
#include "CaptureFrecDriver.h"
#include "I2CDriver.h"
#include "LCDConfig.c"
#include "AdcDriver.h"

/* Definiciones de la variables */

GPIO_Handler_t handlerDetectorR = {0};
GPIO_Handler_t handlerDetectorL = {0};
GPIO_Handler_t handlerDetectorU = {0};
GPIO_Handler_t handlerDetectorD = {0};
GPIO_Handler_t handlerA1 = {0};
GPIO_Handler_t handlerA2 = {0};
GPIO_Handler_t handlerServoDetector = {0};
GPIO_Handler_t handlerDCDetector 	= {0};
GPIO_Handler_t LCDI2cSDA = { 0 };
GPIO_Handler_t LCDI2cSCL = { 0 };
GPIO_Handler_t handlerPinPwmChannel		= {0};
GPIO_Handler_t handlerBoton		= {0};

EXTI_Config_t handlerExtiR= { 0 };
EXTI_Config_t handlerExtiL= { 0 };
EXTI_Config_t handlerExtiU= { 0 };
EXTI_Config_t handlerExtiD= { 0 };

ADC_Config_t 		adcConfigx 		= {0};
ADC_Config_t 		adcConfigy 		= {0};
uint16_t adcValuex = 0;
uint16_t adcValuey = 0;
uint8_t banderAdc=1;

GPIO_Handler_t handlerLedOk				  = {0};
BasicTimer_Handler_t handlerStaterOKTimer = {0};
BasicTimer_Handler_t handlerAdcTimer = {0};
uint16_t delayFlag = 0;

PWM_Handler_t handlerSignalPWM			= {0};
uint16_t duttyValue = 200;

I2C_Handler_t handlerLCD = { 0 };

GPIO_Handler_t handlerPinTX = {0};
GPIO_Handler_t handlerPinRX = {0};
USART_Handler_t handlerCommTerminal = {0};
uint8_t rxData = 0;
char bufferData[64] = "esto es una pequena prueba";

#define LCD_ADDRESS 0x27;
#define izquierda 	0
#define derecha 	1
#define stop 		2
uint8_t DetectorR;
uint8_t DetectorL;
uint8_t DetectorU;
uint8_t DetectorD;
int	 numeroVueltasDC = 0;
int	 numeroVueltasServo = 1;
int numeroVueltasAdcServo =0;
uint8_t rawData = 0;
uint8_t banderaStart=0;
/* definicion de las funciones prototipo */
void initSystem(void);
void girarServo(uint8_t dir);
void girarDC(uint8_t dir);
void detectorRL(void);
void delay(uint8_t H);
void detectorUD(void);
void USARTCommands(void);
void InitLCD();
void joystickControl(void);
/**
 * Funcion principal del programa
 */
int main(void){
	// Llamamos a la funcion que nos inicializa el hardware del sistema
	initSystem();
	InitLCD();
	delay(1);
	MultichannelADC(1);

	/* Main loop */
	while(1){
		//Esperamos a que se pulse el boton para setiar el sistema en altura 0 y activar los detectores
		while(banderaStart==0){
			USARTCommands();
		joystickControl();
		//Aqui se lee si el boton esta pulsado (0 es pulsado)
			if (GPIO_ReadPin(&handlerBoton) == 0) {
				banderaStart = 1;
			}
		}
		USARTCommands();
		//Activamos los sensores del mov circular y tomamos una medida
//		GPIO_WritePin(&handlerDCDetector, SET);
//		detectorRL(); //Aqui se ejecuta la instruccion
		delay(50);
		//Apagamos los sensores circulares y encendemos los verticales
//		GPIO_WritePin(&handlerDCDetector, RESET);
		GPIO_WritePin(&handlerServoDetector, SET);

		detectorUD();//Aqui se ejecuta la instruccion
		delay(50);
//		GPIO_WritePin(&handlerServoDetector, RESET);//Apagamos los sensores verticales


	}
}

/*
 *
 *
 */
void initSystem(void){
	//GPIO///////////////////////////////////////////////////
	//Configuramos la entrada de los detectores///////////////
	handlerDetectorR.pGPIOx								= GPIOC;
	handlerDetectorR.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerDetectorR.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerDetectorR.GPIO_PinConfig.GPIO_PinNumber		= PIN_11;
	handlerDetectorR.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerDetectorR.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerDetectorR);

	handlerDetectorL.pGPIOx								= GPIOC;
	handlerDetectorL.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerDetectorL.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerDetectorL.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerDetectorL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerDetectorL.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerDetectorL);

	handlerDetectorU.pGPIOx								= GPIOA;
	handlerDetectorU.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerDetectorU.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerDetectorU.GPIO_PinConfig.GPIO_PinNumber		= PIN_4;
	handlerDetectorU.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerDetectorU.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerDetectorU);

	handlerDetectorD.pGPIOx								= GPIOB;
	handlerDetectorD.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerDetectorD.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerDetectorD.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
	handlerDetectorD.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerDetectorD.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerDetectorD);

	//Configuramos la polaridad de los motores DC///////////////
	handlerA1.pGPIOx									= GPIOC;
	handlerA1.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerA1.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerA1.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;
	handlerA1.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerA1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerA1.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerA1);

	handlerA2.pGPIOx								= GPIOC;
	handlerA2.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerA2.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerA2.GPIO_PinConfig.GPIO_PinNumber			= PIN_0;
	handlerA2.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerA2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerA2.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerA2);

	//Configuramos los pines de suicheo entre los detectores  del servo y el DC
	handlerServoDetector.pGPIOx									= GPIOC;
	handlerServoDetector.GPIO_PinConfig.GPIO_PinAltFunMode	 	= AF0;
	handlerServoDetector.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerServoDetector.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerServoDetector.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerServoDetector.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerServoDetector.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerServoDetector);
	GPIO_WritePin(&handlerServoDetector, RESET); //Lo llevamos a un estado de no deteccion

	handlerDCDetector.pGPIOx								= GPIOC;
	handlerDCDetector.GPIO_PinConfig.GPIO_PinAltFunMode	 	= AF0;
	handlerDCDetector.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerDCDetector.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	handlerDCDetector.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerDCDetector.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerDCDetector.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerDCDetector);
	GPIO_WritePin(&handlerDCDetector, RESET); //Lo llevamos a un estado de no deteccion

	handlerBoton.pGPIOx								= GPIOB;
	handlerBoton.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerBoton.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_IN;
	handlerBoton.GPIO_PinConfig.GPIO_PinNumber		= PIN_10;
	handlerBoton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerBoton.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerBoton);

	//EXTI///////////////////////////////////////////////////////////////////
	//Le asignamos un canal de exti a cada detector/////////////////////////
	handlerExtiR.pGPIOHandler = &handlerDetectorR;
	handlerExtiR.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerExtiR);

	handlerExtiL.pGPIOHandler = &handlerDetectorL;
	handlerExtiL.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerExtiL);

	handlerExtiU.pGPIOHandler = &handlerDetectorU;
	handlerExtiU.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerExtiU);

	handlerExtiD.pGPIOHandler = &handlerDetectorD;
	handlerExtiD.edgeType = EXTERNAL_INTERRUPT_FALLING_EDGE;
	extInt_Config(&handlerExtiD);

	//USART/////////////////////////////////////////////////////////////
	/* Configuramos los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx								= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPinTX);

	/* configurando los pines sobre los que funciona el USART */
	handlerPinRX.pGPIOx								= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerPinRX);

	// Configuramos la comunicacion serial
	handlerCommTerminal.ptrUSARTx = USART2;
	handlerCommTerminal.USART_Config.USART_baudrate 	= USART_BAUDRATE_9600;
	handlerCommTerminal.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity 		= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits 	= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_enableIntRX	= USART_RX_INTERRUPT_ENABLE;
	USART_Config(&handlerCommTerminal);

	//TIMER/////////////////////////////////////////////////////////////////////
	// Organizamos la configuracion del pin que controla el LED de la board
	handlerLedOk.pGPIOx								= GPIOA;
	handlerLedOk.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF0;
	handlerLedOk.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLedOk.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;		//8
	handlerLedOk.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLedOk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLedOk.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	GPIO_Config(&handlerLedOk);

	// Configuramos el timer
	handlerStaterOKTimer.ptrTIMx 							= TIM2;
	handlerStaterOKTimer.TIMx_Config.TIMx_mode 				= BTIMER_MODE_UP;
	handlerStaterOKTimer.TIMx_Config.TIMx_speed 			= BTIMER_SPEED_1ms;
	handlerStaterOKTimer.TIMx_Config.TIMx_period 			= 250;	//periodod de 250 ms
	handlerStaterOKTimer.TIMx_Config.TIMx_interruptEnable	= BTIMER_INTERRUPT_ENABLE;
	BasicTimer_Config(&handlerStaterOKTimer);
	startTimer(&handlerStaterOKTimer);

	// Configuramos el timer del ADC
	handlerAdcTimer.ptrTIMx						= TIM4;
	handlerAdcTimer.TIMx_Config.TIMx_mode 		= BTIMER_MODE_UP;
	handlerAdcTimer.TIMx_Config.TIMx_speed 		= BTIMER_SPEED_1ms;
	handlerAdcTimer.TIMx_Config.TIMx_period 	= 5;
	BasicTimer_Config(&handlerAdcTimer);
	startTimer(&handlerAdcTimer);

	//PWM///////////////////////////////////////////////////////////////////////
	/* Configuramos el PWM *////////////////////////////////////////////////
	handlerPinPwmChannel.pGPIOx								= GPIOC;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF2;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinPwmChannel.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	//cargamos la config del pin del PWM como ALTFN 2, correspondiente al canal 2 del tim 3
	GPIO_Config(&handlerPinPwmChannel);

	handlerSignalPWM.config.channel 	= PWM_CHANNEL_2;
	handlerSignalPWM.config.duttyCicle 	= duttyValue;
	handlerSignalPWM.config.periodo 	= 200;
	handlerSignalPWM.config.prescaler 	= 1600;
	handlerSignalPWM.ptrTIMx			= TIM3;

	pwm_Config(&handlerSignalPWM);
	enableOutput(&handlerSignalPWM);
	startPwmSignal(&handlerSignalPWM);

	///ADC/////////////////////////////////////////////////////////////
	//Se establecen las configuraciones del ADC
	adcConfigx.channel				= ADC_CHANNEL_1;
	adcConfigx.dataAlignment 		= ADC_ALIGNMENT_RIGHT;
	adcConfigx.resolution 			= ADC_RESOLUTION_12_BIT;
	adcConfigx.samplingPeriod		= 111;///ADC_SAMPLING_PERIOD_15_CYCLES;
	adcConfigx.priority				= 0;

	adcConfigy.channel				= ADC_CHANNEL_0;
	adcConfigy.dataAlignment 		= ADC_ALIGNMENT_RIGHT;
	adcConfigy.resolution 			= ADC_RESOLUTION_12_BIT;
	adcConfigy.samplingPeriod		= 111; //ADC_SAMPLING_PERIOD_15_CYCLES;
	adcConfigy.priority				= 1;

	//Se carga la configuración del ADC
	adc_Config(&adcConfigx);
	adc_Config(&adcConfigy);

	//I2C////////////////////////////////////////////////////////////////
	//I2C config para el LCD/////////////////////////////////////////////
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
	//cargamos la conffiguracion
	handlerLCD.ptrI2Cx = I2C3;
	handlerLCD.modeI2C = I2C_MODE_SM;
	handlerLCD.slaveAddress = LCD_ADDRESS;
	i2c_config(&handlerLCD);

}

/* Callback relacionado con la recepcion del USART2
 * Debo leer el puerto para bajar la bandera de la interupcion
 */
void usart2Rx_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega
	// Esto ademas debe bajar la bandera de la interrupcion
	rxData = getRxData();
}

/**/
void BasicTimer2_Callback(void){
	// Hacemos un blinky para indicar que el equipoesta funcinando correctamente.
	// handlerLedOK
	GPIOxTooglePin(&handlerLedOk);
}

void BasicTimer4_Callback(void){
	// Comienza una convesión ADC
	startSingleADC();
	delayFlag +=1;

}

// ADC (conversión) Callback
void adcComplete_Callback(void){
	// Leemos los valores de la conversión ADC
	if(banderAdc){
		adcValuex = getADC();
		banderAdc = 0;
	}
	else{
		adcValuey = getADC();
		banderAdc = 1;

	}
}

void girarServo(uint8_t dir){
	if(dir == izquierda){
		updateDuttyCycle(&handlerSignalPWM, 181);
		//entre 180 y 185 se mueve hacia la izquierda
	}
	else if(dir == derecha){
		updateDuttyCycle(&handlerSignalPWM, 189);
		//entre 185 y 190 se mueve hacia la derecha
		}
	else if(dir == stop){
		updateDuttyCycle(&handlerSignalPWM, 200);
		//entre 185 y 190 se mueve hacia la derecha
	}
}

void girarDC(uint8_t dir){
	if(dir == izquierda){
		GPIO_WritePin(&handlerA1, SET);
		GPIO_WritePin(&handlerA2, RESET);
	}
	else if(dir == derecha){
		GPIO_WritePin(&handlerA1, RESET);
		GPIO_WritePin(&handlerA2, SET);
		}
	else if(dir == stop){
		GPIO_WritePin(&handlerA1, RESET);
		GPIO_WritePin(&handlerA2, RESET);
	}
}

void detectorRL(void){
	//Leemos los detectores
	DetectorR = GPIO_ReadPin(&handlerDetectorR);
	DetectorL = GPIO_ReadPin(&handlerDetectorL);
	//En el caso de detectar el lado derecho, girar hacia el lado derecho, hasta que se toque el detector Izuiqeda
	if(numeroVueltasDC <28){
		if (DetectorR == 0 && DetectorL == 1) {
			girarDC(derecha);
			writeCmd(&handlerLCD, 0x01);
			delay(1);
			writeString(&handlerLCD, "status: Up");
			while (DetectorL == 1 && numeroVueltasDC < 22) {
				numeroVueltasDC += 1;
				DetectorL = GPIO_ReadPin(&handlerDetectorL);
				delay(1);
			}
			//Luego detenemos giro hasta que quitemos la mano del detector izquierdo
			girarDC(stop);
			while (!DetectorL) {
				DetectorL = GPIO_ReadPin(&handlerDetectorL);
			}
			writeCmd(&handlerLCD, 0x01);
			delay(1);
			writeString(&handlerLCD, "status: Stop");
			delay(1);
		}
	}
	if(numeroVueltasDC > 5){
	if(DetectorR == 1 && DetectorL == 0){
		girarDC(izquierda);
		writeCmd(&handlerLCD, 0x01);
		delay(1);
		writeString(&handlerLCD, "status: Down");
		while(DetectorR ==1 && numeroVueltasDC >5){
			numeroVueltasDC-=1;
			DetectorR = GPIO_ReadPin(&handlerDetectorR);
			delay(1);
		}
		girarDC(stop);
		while(!DetectorR){
			DetectorR = GPIO_ReadPin(&handlerDetectorR);
		}
		writeCmd(&handlerLCD, 0x01);
		delay(1);
		writeString(&handlerLCD, "status: Stop");
		delay(1);
	}
	}
}

void detectorUD(void){
	//Leemos los detectores
	DetectorU = GPIO_ReadPin(&handlerDetectorU);
	DetectorD = GPIO_ReadPin(&handlerDetectorD);
	//En el caso de detectar el lado derecho, girar hacia el lado derecho, hasta que se toque el detector Izuiqeda
	if(numeroVueltasServo <28 && numeroVueltasServo >0) {
		if (DetectorU == 0 && DetectorD == 1) {
			writeCmd(&handlerLCD, 0x01);
			delay(1);
			writeString(&handlerLCD, "status: Up");
			girarServo(derecha);//movi esto
			while (DetectorD == 1 && numeroVueltasServo < 22) {
				numeroVueltasServo += 1;
				DetectorD = GPIO_ReadPin(&handlerDetectorD);
				delay(50);
			}
			//Luego detenemos giro hasta que quitemos la mano del detector izquierdo
			girarServo(stop);
			while (!DetectorD) {
				DetectorD = GPIO_ReadPin(&handlerDetectorD);
			}
			writeCmd(&handlerLCD, 0x01);
			delay(1);
			writeString(&handlerLCD, "status: Stop");
			delay(50);
		}


	if(DetectorU == 1 && DetectorD == 0){
		writeCmd(&handlerLCD, 0x01);
		delay(1);
		writeString(&handlerLCD, "status: Down");
		girarServo(izquierda);
		while(DetectorU ==1 && numeroVueltasServo >1){
			numeroVueltasServo-=1;
			DetectorU = GPIO_ReadPin(&handlerDetectorU);
			delay(50);
		}
		girarServo(stop);
		while(!DetectorU){
			DetectorU = GPIO_ReadPin(&handlerDetectorU);
		}
		writeCmd(&handlerLCD, 0x01);
		delay(1);
		writeString(&handlerLCD, "status: Stop");
		delay(50);

	}
	}
	else{
		__NOP();
	}
}


void joystickControl(void){
	if(adcValuey < 500 && adcValuex >1500 && adcValuex < 2500 ){
		girarServo(derecha);
		while(adcValuey <500){
			numeroVueltasAdcServo += 1;
			delay(1);
		}
		girarServo(stop);
	}
	else if(adcValuey > 3500 && adcValuex >1500 && adcValuex < 2500){
		girarServo(izquierda);
		while(adcValuey > 3500){
			numeroVueltasAdcServo -= 1;
			delay(1);

		}
		girarServo(stop);
	}
	else if(adcValuex > 3500 && adcValuey >1500 && adcValuey < 2500){
		girarDC(derecha);
		delay(1);
		girarDC(stop);
	}
	else if(adcValuex < 500 && adcValuey >1500 && adcValuey < 2500){
		girarDC(izquierda);
		delay(1);
		girarDC(stop);
	}
	else{
		__NOP();
	}
}
//Secuencia de inicializacion
void InitLCD(){
	delay(1);
	writeCmd(&handlerLCD, 0x30);
	delay(1);
	writeCmd(&handlerLCD, 0x30);
	delay(1);
	writeCmd(&handlerLCD, 0x32);
	writeCmd(&handlerLCD, 0x28);
	writeCmd(&handlerLCD, 0x0C);
	writeCmd(&handlerLCD, 0x01);
	writeCmd(&handlerLCD, 0x06);
}

//Usando el Timer 2, creamos un delay. 1 ciclo es 1/64 segundo
void delay(uint8_t H){
	delayFlag = 0;
	while(delayFlag<H){
		__NOP();
	}
}
void USARTCommands(void){
	//Hacemos un eco con el valor que nos llega por el serial
	if(rxData != '\0'){
		if(rxData == 'm'){
			writeMsg(&handlerCommTerminal, "TallerVRocks! \n");
			rxData = '\0';
		}
		//probar DC
		else if(rxData == 'r'){
			writeMsg(&handlerCommTerminal, "derecha \n");
			girarDC(derecha);
			rxData = '\0';
		}
		else if(rxData == 'l'){
			writeMsg(&handlerCommTerminal, "izquierda \n");
			girarDC(izquierda);
			rxData = '\0';
		}
		else if(rxData == 'k'){
			writeMsg(&handlerCommTerminal, "stop \n");
			girarDC(stop);
			rxData = '\0';
		}

		//Probar Servo
		else if(rxData == 'd'){
			writeMsg(&handlerCommTerminal, "derecha \n");
			girarServo(derecha);
			rxData = '\0';
		}
		else if(rxData == 'i'){
			writeMsg(&handlerCommTerminal, "izquierda \n");
			girarServo(izquierda);
			//entre 180 y 185 se mueve hacia la izquierda
			rxData = '\0';
		}
		else if(rxData == 'p'){
			writeMsg(&handlerCommTerminal, "pausa \n");
			girarServo(stop);
			//entre 180 y 185 se mueve hacia la izquierda
			rxData = '\0';
		}
		else if(rxData == 'o'){
			sprintf(bufferData, "vueltas = %d \n", (int) numeroVueltasServo);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';

		}
		else if(rxData == 'q'){
			GPIO_WritePin(&handlerDCDetector, SET);
			GPIO_WritePin(&handlerServoDetector, SET);

			writeMsg(&handlerCommTerminal, "setServo \n");
			rxData = '\0';
		}
		else if(rxData == 's'){
			sprintf(bufferData, "XValue = %u \n", adcValuex);
			writeMsg(&handlerCommTerminal, bufferData);
			sprintf(bufferData, "YValue = %u \n", adcValuey);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';
		}
		else if(rxData == 'j'){
			writeString(&handlerLCD, "status: Giro izquierda");
			writeMsg(&handlerCommTerminal, "preuba LCD \n");
			rxData = '\0';
		}
		else if(rxData == 'h'){
			rawData = GPIO_ReadPin(&handlerDetectorD);
			sprintf(bufferData, "estado = %u \n", rawData);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';

		}
		else if(rxData == 'f'){
			rawData = GPIO_ReadPin(&handlerBoton);
			sprintf(bufferData, "boton = %u \n", rawData);
			writeMsg(&handlerCommTerminal, bufferData);
			rxData = '\0';
		}

		rxData = '\0';
	}
}
