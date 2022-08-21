/*
 * main.c
 *
 *  Created on: Aug 20, 2022
 *      Author: Camilo
 */

#include <stdint.h>

   short dias_transcurridos;
   int   horas_transcurridos;
   int   segundos_transcurridos;

main() {
	   int years  =  2022;
	   int months =  9;
	   int days   =  10;
	   int horas  =  5;

    /* El numero de dias entre 2001 y 2021 = (years-1-2000)*365
     * El numero de dias en el 2000 = (12-months+1)*31-(days)
     * El numero de dias transcurridos hasta el 7 de agosto =(7)*31+ (31-17) */

   dias_transcurridos = ((years-1-2000)*365+(12-months)*31+(30-days) + (7)*31+ (31-17))-1;
   horas_transcurridos = dias_transcurridos*24 + horas;
   segundos_transcurridos = dias_transcurridos*3600 + horas*3600;

   printf("%d",dias_transcurridos);
   printf("\n%d",horas_transcurridos);
   printf("\n%d",segundos_transcurridos);

}
