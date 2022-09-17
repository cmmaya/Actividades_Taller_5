
/*los punteros simpre son de 32 bits
*/

#include  <stdint.h>

int main(void){

	// 1.0 Ejemplo

	uint8_t dato = 124;

	// Creacion de un puntero
	uint8_t *pDato;	// Se coloca el tama~no del dato que se va a almacenar

	// Asignando el valor en memoria al puntero
	// El ampersand entrega la direccio'n como un "*uint32_t"
	pDato = &dato;

	// Crear una variable para almacenar la direccio'n de memoria.
	//CASTEO: forzar una variable a cambiar el tama~no
	uint32_t direccionDato = (uint32_t) pDato;
	(void) direccionDato;

	// Para escribir en el contenido de la direccio'n de memoria, se usa el puntero.
	*pDato = 200;

	// Pasar a ala siguiente direccion de memoria
	pDato++;

	uint8_t nuevoDato = *pDato;
	(void) nuevoDato;

	// Crear variable de 16 bits
	uint16_t variable = 12;

	// Creamos un puntero para variable
	uint16_t *pVariable;

	// Asignar direccion de memoria
	pVariable = &variable;

	// Direccion de memoria
	uint32_t dir = (uint32_t) pVariable;
	(void) dir;

	pVariable++;

	// Guardar un nuevo valor a traves del puntero
	*pVariable = 12;

	//2.0 ARREGLOS: conjunto de datos del mismo tipo.
	// Son un puntero al primer elemento del arreglo.

	#define sizeOfArray 4

	uint8_t miPrimerArreglo[sizeOfArray] = {5, 0xA, 'a', 254};

	// Recorrer los arreglos
	for(uint8_t i = 0; i < sizeOfArray; i++){
		uint8_t contenido = miPrimerArreglo[i];
	}

	//2.2 Recorrer arreglos con punteros
	for(uint8_t i = 0; i<sizeOfArray; i++)
	{
		uint8_t contenido = *(miPrimerArreglo + i);
	}

	//3.0 Estructuras, no son punteros

	//DEfinicion general

	typedef struct{
		uint8_t  elemento1;
		uint16_t elemento2;
		uint32_t elemento3;
		uint8_t arreglo[sizeOfArray];
	}miPrimeraEstructura_t;

	//Inicializando la estructura
	miPrimeraEstructura_t estructuraEjemplo = {0};

	//Asignamos valores
	estructuraEjemplo.elemento1 = 'F';
	estructuraEjemplo.arreglo[0]= 10;

	//Como acceder a las estructuras co punteros

	miPrimeraEstructura_t *ptrMiPrimeraEstructura;

	ptrMiPrimeraEstructura->elemento1 = 9;
	ptrMiPrimeraEstructura->arreglo[0] = 3;
	ptrMiPrimeraEstructura->elemento3 = 4;

}
