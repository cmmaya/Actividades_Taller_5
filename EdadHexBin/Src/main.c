#include <stdint.h>
#include<stdio.h>
#include<stdlib.h>
//////PUNTO 1/////////


main(void) {
   unsigned short dias_transcurridos = 0;
   unsigned int   horas_transcurridos = 0;
   unsigned int   segundos_transcurridos = 0;
   unsigned short years  =  2022;
   unsigned char months =  9;
   unsigned char days   =  10;
   unsigned char horas  =  5;

    /* El numero de dias entre 2001 y 2021 = (years-1-2000)*365
     * El numero de dias en el 2000 = (12-months+1)*31-(days)
     * El numero de dias transcurridos hasta el 7 de agosto =(7)*31+ (31-17)
     * No se cuenta el ultimo dia, pues esta incompleto*/


/////PUNTO 2////////
   dias_transcurridos = ((years-1-2000)*365+(12-months)*31+(30-days) + (7)*31+ (31-17))-1;


/////PUNTO 3////////
   /*para las horas multiplico los dias por 24 y sumo las horas del dia que falta*/
   horas_transcurridos = dias_transcurridos*24 + horas;

/////PUNTO 4////////
   /*para los segundos multiplico las horas por 3600*/
   segundos_transcurridos = dias_transcurridos*3600 + horas*3600;


/*imprimo los valores*/
   printf("%d",dias_transcurridos);
   printf("\n%d",horas_transcurridos);
   printf("\n%d",segundos_transcurridos);
   printf("\n","");

 /////PUNTO 5////////
   int i,n,h,k;
   int a[13],b[18],c[7];
 /* Para transformar el numero a binario almaceno progresivamente el residuo de el numero inicial dividido por 2 hasta que ya no se pueda dividir
 * para poder ver el numero en el debugger lo almaceno en una matriz a[]*/


   n = dias_transcurridos;
   for(i=0;n>0;i++)
   {
   a[i]=n%2;
   n=n/2;
   }
   /*anoto el numero resultante dias_binario= 1111101001000*/
   unsigned long dias_binario= 1111101001000;



   /*hacemos lo mismo para las horas transcurridas, usando la matriz b[]*/
      printf("\n","");
   h = horas_transcurridos;

   for(i=0;h>0;i++)
   {
   b[i]=h%2;
   h=h/2;
   }
   /*anoto el numero resultante tambien
   horas_binario=101110111011000101*/


      /*Para obtener el hexadecimal guardamos en la matriz c[] los modulos del numero divido por 16 hasta que ya no se pueda seguir dividiendo*/
   printf("\n","");
   k = segundos_transcurridos;
   for(i=0;k>0;i++)
   {
   c[i]=k%16;
   k=k/16;
   }
      /*El resultado es 1 11 8 2 10 13 0
   reemplazando los numeros por sus correspondientes letras queda
   1B82AD0*/


/////PUNTO 6////////
   dias_transcurridos=dias_transcurridos<<1;
   dias_transcurridos=dias_transcurridos<<1;
   dias_transcurridos=dias_transcurridos<<1;
   dias_transcurridos=dias_transcurridos<<1;
   /*explicacion: Al aplicar la operacion es como multiplicar por 2 el valor inicial, al seguir ejecutando la operacion el valor se multiplica por 2^n. 4 veces equivale a multiplicar por 16*/
/////PUNTO 7////////
   horas_transcurridos=horas_transcurridos>>1;
   horas_transcurridos=horas_transcurridos>>1;
   horas_transcurridos=horas_transcurridos>>1;
   horas_transcurridos=horas_transcurridos>>1;
   /*explicacion: Al aplicar la operacion es como dividir por 2 el valor inicial, al seguir ejecutando la operacion el valor se divide por 2^n. 4 veces equivale a dividir por 16*/

/////PUNTO 8////////
   /*retomo el valor original de la variable dias_transcurridos*/
   dias_transcurridos = 8008;
   /*Lo invertimos: bitwise NOT*/
   dias_transcurridos= ~dias_transcurridos;
   /*sumamos 1 */
   dias_transcurridos= dias_transcurridos+1;
   /*sumamos el valor original*/
   dias_transcurridos= dias_transcurridos+8008;

  /*EL RESULTADO DA 0 PARA CUALQUIER VARIABLE*/


/////PUNTO 9////////

  /*El numero de la 4ta posicio

   printf("%d", dias_transcurridos);
}
