#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
//////PUNTO 1/////////
//CAMILO MONSALVE M.//
//FECHA DE NACIMIENTO 10 de SEP del 2000///

main(void) {

	unsigned short dias_transcurridos = 0;
	unsigned int horas_transcurridos = 0;
	unsigned int segundos_transcurridos = 0;
	unsigned short years = 2022;
	unsigned char months = 9;
	unsigned char days = 10;
	unsigned char horas = 5;

/////PUNTO 2////////

	/* El numero de dias entre 2001 y 2021 = (years-1-2000)*365
	 * El numero de dias en el 2000 = (12-months+1)*31-(days)
	 * El numero de dias transcurridos hasta el 7 de agosto =(7)*31+ (31-17)
	 * No se cuenta el ultimo dia, pues esta incompleto*/
	dias_transcurridos = ((years - 1 - 2000) * 365 + (12 - months) * 31
			+ (30 - days) + (7) * 31 + (31 - 17)) - 1;
	// Este calculo debia escribirse con variables y solo entregar el valor final
	// No se solicitaba la hora de nacimiento.

/////PUNTO 3////////
	/*para las horas multiplico los dias por 24 y sumo las horas del dia que falta*/
	horas_transcurridos = dias_transcurridos * 24 + horas;

/////PUNTO 4////////
	/*para los segundos multiplico las horas por 3600*/
	segundos_transcurridos = dias_transcurridos * 3600 + horas * 3600;
	// De nuevo, acá hacen falta variables definidas correctamente.

	/////PUNTO 5////////
	int i, n, h, k;
	int a[13], b[18], c[7];
	/* Para transformar el numero a binario almaceno progresivamente el residuo de el numero inicial dividido por 2 hasta que ya no se pueda dividir
	 * para poder ver el numero en el debugger lo almaceno en una matriz a[]*/

	n = dias_transcurridos;
	for (i = 0; n > 0; i++) {
		a[i] = n % 2;
		n = n / 2;
	}
	/*anoto el numero resultante: dias_binario= 1111101001000*/

	/*hacemos lo mismo para las horas transcurridas, usando la matriz b[]*/

	h = horas_transcurridos;

	for (i = 0; h > 0; i++) {
		b[i] = h % 2;
		h = h / 2;
	}
	/*anoto el numero resultante tambien:
	 horas_binario=101110111011000101*/

	/*Para obtener el hexadecimal guardamos en la matriz c[] los modulos del numero divido por 16 hasta que ya no se pueda seguir dividiendo*/
	k = segundos_transcurridos;
	for (i = 0; k > 0; i++) {
		c[i] = k % 16;
		k = k / 16;
	}
	/*El resultado es 1 11 8 2 10 13 0
	 reemplazando los numeros por sus correspondientes letras queda
	 1B82AD0*/
	// El uso de ciclos for, while, if etc no estaba permitido. Era solo un ejercicio matematico simple.
	// Tus resultados no son correctos.

/////PUNTO 6////////
	dias_transcurridos = dias_transcurridos << 1;
	dias_transcurridos = dias_transcurridos << 1;
	dias_transcurridos = dias_transcurridos << 1;
	dias_transcurridos = dias_transcurridos << 1;
	/*explicacion: Al aplicar la operacion es como multiplicar por 2 el valor inicial, al seguir ejecutando la operacion el valor se multiplica por 2^n. 4 veces equivale a multiplicar por 16
	 *esta operacion funciona porque al desplazar los numeros una posicion hacia la izquierda y poner un 0 al principio, estamos elevando todos los numeros a una potencia de 2 mas, que es lo mismo que multiplicar por 2, ver el ejemplo:
	 011 = 1*2^0+1*2^1+0*2^2
	 <<011 =  110 = 0*2^0+1*2^1+1*2^2  =  2*(1*2^0+1*2^1+0*2^2)
	 */
/////PUNTO 7////////
	horas_transcurridos = horas_transcurridos >> 1;
	horas_transcurridos = horas_transcurridos >> 1;
	horas_transcurridos = horas_transcurridos >> 1;
	horas_transcurridos = horas_transcurridos >> 1;
	/*explicacion: Al aplicar la operacion es como dividir por 2 el valor inicial, al seguir ejecutando la operacion el valor se divide por 2^n. 4 veces equivale a dividir por 16
	 *esta operacion funciona porque al desplazar los numeros una posicion hacia la izquierda y poner un 0 al principio, estamos elevando todos los numeros a una potencia de 2 menos, que es lo mismo que dividir por 2, ver el ejemplo:
	 110 = 1*2^0+1*2^1+0*2^2
	 <<110 =  011 = 0*2^2+1*2^1+1*2^0  =  (1/2)*(1*2^2+1*2^1+0*2^0)   */

/////PUNTO 8////////
	/*retomo el valor original de la variable dias_transcurridos*/
	dias_transcurridos = 8008;
	/*Lo invertimos: bitwise NOT*/
	dias_transcurridos = ~dias_transcurridos;
	/*sumamos 1 */
	dias_transcurridos = dias_transcurridos + 1;
	/*sumamos el valor original*/
	dias_transcurridos = dias_transcurridos + 8008;

	/*EL RESULTADO DA 0 PARA CUALQUIER VARIABLE,
	 pues al sumar el numero con su inverso da un numero con solo valores de 1 e.g 1010+ 0101 =1111.   Luego al sumarle 1, al estar completo el numero de bits(pues se le especifico en el formato), se reinicia el ciclo(se vuelven ceros) */
	// Tus numeros binarios y hexadecimales estan mal escritos. para un bin debes escribir el prefijo 0b, para un hex debes escribir el prefijo 0x.

/////PUNTO 9////////
	/*El numero de la 4ta posicion de 1B82AD0  es 8, al ser par, vamos a obtener los valores de la posicion 1 y 5 y anular las demas
	 *PRIMERO vamos a convertir nuestro numero HEX a binario: Recordemos que cada numero HEX se almacena con 4 bits, entonces quedaria
	 1   |B   |8   |2   |A   |D   |0   |
	 0001|1011|1000|0010|1010|1101|0000|
	 Para anular la Primera(D) y la quinta (B) posicion utilizamos el operador AND de la siguiente forma:

	 1010&
	 1100
	 =1000
	 En nuestro numero aplicamos la misma logica:
	 0001|1011|1000|0010|1010|1101|0000|&
	 0001|0000|1000|0010|1010|0000|0000|

	 el numero que utilizamos de mascara es 0001000010000010101000000000 y corresponde a 1082A00

	 La manera mas facil de verlo, es que al utilizar la operacion AND, los bits 0 anulan los 1, por lo que, al cambiar la letra por 0, sirve como mascara para anular la posicion que queremos
	 Comprobemoslo.

	 El numero con el que arrancas la operación no es correcto, es un efecto del calculo erroneo en el punto 5.
	 */
	int mask1;
	mask1 = 0x1082a00 & 0x1b82ad0;
	/*el numero resultante es 17312256, que es 0x1082a00 en hex*/

/////PUNTO 10////////
	int mask2;
	mask2 = 0x040 & 0x1b82ad0;
	/*El numero es distinto de 0, procedemos a borrar el numero de la posicion 6 usando la misma logica del punto anterior*/
	int mask3;
	mask3 = 0x0b82ad0 & 0x1b82ad0;
	// El numero con el que arrancas la operación no es correcto, es un efecto del calculo erroneo en el punto 5.
	/*El resultante es 0x0b82ad0*/

//////PUNTO 11//////
	horas_transcurridos = !horas_transcurridos;
	horas_transcurridos = !horas_transcurridos;
	/*Al aplicar la primera vez la operacion, el valor de la variable se vuelve 0, y si se aplica de nuevo se vuelve 1. Esto porque el ! es un operador NOT, que vuelve 0 las variables, al negar la variable 0, da como equivalente 1, porque negar el 0, da el resultado contrario, es decir, 1.*/

	// Al compilar me salen 7 warnings!!!
	// Esto se advirtio desde el inicio que no era aceptable.
}
