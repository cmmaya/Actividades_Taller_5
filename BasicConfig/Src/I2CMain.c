#include <stdint.h>

#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "PWMDriver.h"

PWM_Handler_t 		handlerPWM = {0};
GPIO_Handler_t       handlerPIN = {0};
GPIO_Handler_t       handlerBlinky = {0};
BasicTimer_Handler_t handlerTimer2 = {0};