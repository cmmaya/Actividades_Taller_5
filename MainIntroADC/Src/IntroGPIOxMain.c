
#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"


BasicTimer_Handler_t handlerTimer2 = {0};
GPIO_Handler_t       handlerBlinky = {0};


uint8_t blinkState = 0;

int main(void){


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

