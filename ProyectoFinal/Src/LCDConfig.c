//#include <stm32f4xx.h>
//#include <stdint.h>
//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <stdbool.h>
//#include <math.h>
//
//#include "GPIOxDriver.h"
//#include "BasicTimer.h"
//#include "USARTxDriver.h"
//#include "I2CDriver.h"
//
//void writeCmd(I2C_Handler_t *handlerLCD, uint8_t cmd){
//	uint8_t cmdMS = cmd & 0xf0;
//	uint8_t cmdLS = (cmd<<4) & 0xf0;
//
//	uint8_t dato1 = cmdMS | 0x0C; //rs =0 rw=1 en=1
//	uint8_t dato2 = cmdMS | 0x08; //rs =0 rw=1 en=0
//	uint8_t dato3 = cmdLS | 0x0C;
//	uint8_t dato4 = cmdLS | 0x08;
//
//	lcd_writeSingleRegister(handlerLCD, dato1);
//	lcd_writeSingleRegister(handlerLCD, dato2);
//	lcd_writeSingleRegister(handlerLCD, dato3);
//	lcd_writeSingleRegister(handlerLCD, dato4);
//}
////Funcion para andar datos a la pantalla en 4 bit (rs=1)
//void writeData(I2C_Handler_t *handlerLCD,uint8_t cmd){
//	uint8_t cmdMS = cmd & 0xf0;
//	uint8_t cmdLS = (cmd<<4) & 0xf0;
//
//	uint8_t dato1 = cmdMS | 0x0D; //rs =1 rw=1 en=1
//	uint8_t dato2 = cmdMS | 0x09; //rs =1 rw=1 en=0
//	uint8_t dato3 = cmdLS | 0x0D;
//	uint8_t dato4 = cmdLS | 0x09;
//
//	lcd_writeSingleRegister(handlerLCD, dato1);
//	lcd_writeSingleRegister(handlerLCD, dato2);
//	lcd_writeSingleRegister(handlerLCD, dato3);
//	lcd_writeSingleRegister(handlerLCD, dato4);
//}
