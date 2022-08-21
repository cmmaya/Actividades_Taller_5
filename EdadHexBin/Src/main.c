/*
 * main.c
 *
 *  Created on: Aug 20, 2022
 *      Author: Camilo
 */
#include <stdint.h>
#include<stdio.h>
#include<stdlib.h>

   short dias_transcurridos;
   int   horas_transcurridos;
   int   segundos_transcurridos;

main() {
	   short years  =  2022;
	   char months =  9;
	   char days   =  10;
	   char horas  =  5;

    /* El numero de dias entre 2001 y 2021 = (years-1-2000)*365
     * El numero de dias en el 2000 = (12-months+1)*31-(days)
     * El numero de dias transcurridos hasta el 7 de agosto =(7)*31+ (31-17)
     * No se cuenta el ultimo dia, pues esta incompleto*/

   dias_transcurridos = ((years-1-2000)*365+(12-months)*31+(30-days) + (7)*31+ (31-17))-1;
   /*para las horas multiplico los dias por 24 y sumo las horas del dia que falta*/
   horas_transcurridos = dias_transcurridos*24 + horas;
   /*para los segundos multiplico las horas por 3600*/
   segundos_transcurridos = dias_transcurridos*3600 + horas*3600;

   printf("%d",dias_transcurridos);
   printf("\n%d",horas_transcurridos);
   printf("\n%d",segundos_transcurridos);
   printf("\n","");


 /* Para transformar el numero a binario, primero inicializo una matriz de 32 espacios y las variables n e i que vamos a utilizar en el for
 *Luego en un ciclo for, almaceno progresivamente el residuo de el numero inicial dividido por 2
 *Repito el ciclo con el numero dividido entre 2 hasta que ya no se pueda dividir */
   int a[32],i,n;
   n = dias_transcurridos;
   for(i=0;n>0;i++)
   {
   a[i]=n%2;
   n=n/2;
   /*Imprimo cada elemento de la matriz concatenado*/
   }
   for(i=i-1;i>=0;i--)
   {
   printf("%d",a[i]);
   }
   }
