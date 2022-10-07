
#include <stdint.h>


//1.0 Paso por valor basico
void duplicarNumero(uint8_t numero){
	numero *= 2;
}
//1.1 Paso por referencia
void duplicarNumeroRef(uint8_t *numero){
	*numero = (*numero) * 2;
}
uint8_t duplicarNumeroReturn(uint8_t numero){
	return (numero) * 2;
}
void abonarDeudas(uint16_t misDeudas[], uint8_t cantidadDeudas){
	for(uint8_t i = 0; i < cantidadDeudas; i++){

	}
}

void stringCaseConverter(uint8_t *string){
	uint8_t i = 0;
	while(string[i] != '\0'){
		if(string[i]<91 && string[i]>64){
			string[i] = string[i]+32;
			i+=1;

		}
		else if(string[i]>96 && string[i]<122){
			string[i] = string[i]-32;
			i+=1;
		}
	}



}

int main(void){
	uint8_t n =10;
	//pasamos por valor
	duplicarNumero(n);


	uint8_t m =10;
	//pasamos por referencia
	duplicarNumeroRef(&m);

	n=12;
	m=14;

	n=duplicarNumeroReturn(n);
	n=	50;

	uint8_t miPrimerSring[] = "Hola mundo";
	n=0;


	uint8_t palabra[] = "HolAMunDO";

	stringCaseConverter(&palabra);

}
