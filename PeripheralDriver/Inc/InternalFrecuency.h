/*
 * internalFrecuency.h
 *
 *  Created on: Nov 11, 2022
 *      Author: Camilo
 */

#ifndef INTERNALFRECUENCY_H_
#define INTERNALFRECUENCY_H_

#include <stm32f4xx.h>
#include "USARTxDriver.h"



void internalFrecuency_config(void);
void setFrecuency_Mhz(uint8_t frecuency);
#endif /* INTERNALFRECUENCY_H_ */
