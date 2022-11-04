/*
 * I2CDriver.c
 */

#include <stdint.h>
#include "I2CDriver.h"

/*
 * Recordar que se debe configurar los pines para el I2C
 * (SDA y SCL) para lo cual se necesita el modulo GPIO y los pines
 * configurados. Ademas, estos pines deben ser configurados como
 * salidas OpenDrain y con las resistencias en modo Pull-up
 */

void i2c_config(I2C_Handler_t	*ptrHandlerI2C){
	//Activamos la senal de reloj para el modulo i2c sleccionado
	if(ptrHandlerI2C->ptrI2Cx == I2C1){
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	}else if(ptrHandlerI2C->ptrI2Cx == I2C2){
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

	}else if(ptrHandlerI2C->ptrI2Cx == I2C3){
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}

	// 2. Reiniciamos el periferico, de forma que inicia en un estado conocido
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_SWRST;

	/* 3. Indicamos cual es la velocidad del reloj principal, que es la senhal utilizada
	 * por el periferico para generar la senhal de reloj para el bus I2C
	 */
	ptrHandlerI2C->ptrI2Cx->CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos); // Borramos la configuracion previa
	ptrHandlerI2C->ptrI2Cx->CR2 |= (MAIN_CLOCK_16_MHz_FOR_I2C << I2C_CR2_FREQ_Pos);

	// 4. configuramos el modo I2C en el que el sistema funcione. En esta configuracion se incliye tambien
	// la velocidad de reloj y el tiempo maximo para el cambio de la senhal (Trise). Todo comienza con
	// los 2 registros en 0

	ptrHandlerI2C->ptrI2Cx->CCR = 0;
	ptrHandlerI2C->ptrI2Cx->TRISE = 0;

	if(ptrHandlerI2C->modeI2C == I2C_MODE_SM){
		// Estamos en modo "standar" (SM mode)
		// Seleccionamos el modo standar
		ptrHandlerI2C->ptrI2Cx->CCR &= ~I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la senhal del reloj
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);

		// configuramos el registro que controla el tiempo T-rise maximo
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_SM;

	}else{
		// Estamos en modo "fast" (FM mode)
		// Seleccionamos el mod Fast
		ptrHandlerI2C->ptrI2Cx->CCR |= I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la senhal del reloj
		ptrHandlerI2C->ptrI2Cx->CCR |= (I2C_MODE_SM_SPEED_400KHz << I2C_CCR_CCR_Pos);

		// configuramos el registro que controla el tiempo T-rise maximo
		ptrHandlerI2C->ptrI2Cx->TRISE |= I2C_MAX_RISE_TIME_FM;

	}

	// 5. Activamos el modilo I2C
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_PE;

}

// 8. generamos la condicion deStop
void i2c_stopTransaction(I2C_Handler_t *ptrHandlerI2C){
	// 7. Generamos la condicion Stop
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_STOP;
}

/* 1. verificamos que l linea no este ocupada - bit "busy" en I2C_CR2
 * 2. Generamos la senhal start
 * 2a. esperamos que la vandera del evento start se levante
 * Mientras esperamos, el valor de SB es 0, entonces la negacion (!) es 1
 */
void i2c_startTransaction(I2C_Handler_t *ptrHandlerI2C){
	while(ptrHandlerI2C->ptrI2Cx->SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	// 2. Generamos la senhal start
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	// 2a. esperamos que la vandera del evento start se levante
	// Mientras esperamos, el valor de SB es 0, entonces la negacion (!) es 1
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}

void i2c_restartTransaction(I2C_Handler_t *ptrHandlerI2C){
	// 2. Generamos la senhal start
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_START;

	// 2a. esperamos que la vandera del evento start se levante
	// Mientras esperamos, el valor de SB es 0, entonces la negacion (!) es 1
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_SB)){
		__NOP();
	}
}


// 7a. Activamos la indicacion para No-ACK (indicacion para el slave de terminar)
void i2c_sendNoAck(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 &= ~I2C_CR1_ACK;
}

// 7b. Activamos la indicacion para ACK (indicacion para el slave de terminar)
void i2c_sendAck(I2C_Handler_t *ptrHandlerI2C){
	ptrHandlerI2C->ptrI2Cx->CR1 |= I2C_CR1_ACK;
}

void i2c_sendSlaveAddressRW(I2C_Handler_t *ptrHandlerI2C, uint8_t slaveAddress, uint8_t readOrWrite){
	// 0. Definimos una variable auxiliar
	uint8_t auxByte = 0;
	(void) auxByte;

	//3. Enviamos la direccion del Slave y del Bit que indica que deseamos escribir (0)
	// En el siguiente paso se envia la idreecion de memoria que se desea escribir
	ptrHandlerI2C->ptrI2Cx->DR = (slaveAddress << 1) | readOrWrite;

	// 3.1 Esperamos hasta que la bandera del evento "Addr" se levanta
	// Esto nos indica que la direccion fue enviada satisfactioriamente
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_ADDR)){
		__NOP();
	}

	// 3.2 Debemos limpiar la bandera de la recepcion de ACK en la "Addr", para lo cual debemos
	// leer en secuencia priemero el SR1 y luego el SR2

	auxByte = ptrHandlerI2C->ptrI2Cx->SR1;
	auxByte = ptrHandlerI2C->ptrI2Cx->SR2;
}

void i2c_sendMemoryAddress(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	// 4. Enviamos la direccion de memoria que deseamos leer
	ptrHandlerI2C->ptrI2Cx->DR = memAddr;

	// 4.1 Esperamos hasta que el Byte sea transmitido
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_TXE)){
		__NOP();
	}
}

void i2c_sendDataByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	// 5. Enviamos la direccion de memoria que deseamos escribir
	ptrHandlerI2C->ptrI2Cx->DR = dataToWrite;

	// 6. Esperamos hasta que el Byte sea transmitido
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_BTF)){
		__NOP();
	}
}

uint8_t i2c_readDataByte(I2C_Handler_t *ptrHandlerI2C){
	while( !(ptrHandlerI2C->ptrI2Cx->SR1 & I2C_SR1_RXNE)){
		__NOP();
	}
	 ptrHandlerI2C->dataI2C = ptrHandlerI2C->ptrI2Cx->DR;

	 return ptrHandlerI2C->dataI2C;
}

uint8_t i2c_readSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead){
	//Creamos variable aux
	uint16_t auxRead = 0;

	//Generamos el start
	i2c_startTransaction(ptrHandlerI2C);

	//Enviamos la direccion del esclavo e indicacion escribir
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//Enviamos la direccion de memoria
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	//Creamos la condicion de restart
	i2c_restartTransaction(ptrHandlerI2C);

	//Enviamos la direccion del exclavo y la indicacon LERR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	//mandamos el send no ack, pa que avabe
	i2c_sendNoAck(ptrHandlerI2C);

	//Generamos la condicion STOP, pa que se detenga el slave despues de un byte
	i2c_stopTransaction(ptrHandlerI2C);
//	//Leemos el dato que envia el esclave
//	auxRead = i2c_readDataByte(ptrHandlerI2C) << 8;
//
//	//mandamos el ack de nuevo
//	i2c_sendAck(ptrHandlerI2C);
//
//	//leemos de nuevo
//	auxRead |= i2c_readDataByte(ptrHandlerI2C);
//
//	//mandamos el send no ack, pa que avabe
//	i2c_sendNoAck(ptrHandlerI2C);
//
//	//Generamos la condicion STOP, pa que se detenga el slave despues de un byte
//	i2c_stopTransaction(ptrHandlerI2C);

	auxRead = i2c_readDataByte(ptrHandlerI2C);
	return auxRead;

}

void i2c_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t regToRead, uint8_t newValue){
	//Genermos la condicion de start
	i2c_startTransaction(ptrHandlerI2C);

	//Enviamos la direccion del esclavo
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//Enviamos la direccion de memoria
	i2c_sendMemoryAddress(ptrHandlerI2C, regToRead);

	//Enviamos el valor que deseamos escribir
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	//Condicion de stop
	i2c_stopTransaction(ptrHandlerI2C);

}

void lcd_writeSingleRegister(I2C_Handler_t *ptrHandlerI2C, uint8_t newValue){
	//Genermos la condicion de start
	i2c_startTransaction(ptrHandlerI2C);

	//Enviamos la direccion del esclavo
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_WRITE_DATA);

	//Enviamos el valor que deseamos escribir
	i2c_sendDataByte(ptrHandlerI2C, newValue);

	//Condicion de stop
	i2c_stopTransaction(ptrHandlerI2C);

}

uint8_t lcd_readSingleRegister(I2C_Handler_t *ptrHandlerI2C){
	//Creamos variable aux
	uint16_t auxRead = 0;

	//Generamos el start
	i2c_startTransaction(ptrHandlerI2C);

	//Enviamos la direccion del exclavo y la indicacon LERR
	i2c_sendSlaveAddressRW(ptrHandlerI2C, ptrHandlerI2C->slaveAddress, I2C_READ_DATA);

	//mandamos el send no ack, pa que avabe
	i2c_sendNoAck(ptrHandlerI2C);

	//Generamos la condicion STOP, pa que se detenga el slave despues de un byte
	i2c_stopTransaction(ptrHandlerI2C);


	auxRead = i2c_readDataByte(ptrHandlerI2C);
	return auxRead;

}
