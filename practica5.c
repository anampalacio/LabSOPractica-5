#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//VARIABLES GLOBALES
int *arregloCuentas;

void *transferencia(int cuentas[]){
	int cuentaOrigen = cuentas[0];
	int cuentaDestino = cuentas[1];
	srand(time(NULL));
	int montoTransferir = 0;
	if(arregloCuentas[cuentaOrigen] != 0){
		montoTransferir = (rand() % (arregloCuentas[cuentaOrigen]+1));
		printf("1. Monto a transferir: %d - Cuenta Origen: %d - Cuenta Destino %d \n",montoTransferir,cuentaOrigen+1,cuentaDestino+1);
		arregloCuentas[cuentaOrigen] = arregloCuentas[cuentaOrigen]-montoTransferir;
		arregloCuentas[cuentaDestino] =  arregloCuentas[cuentaDestino]+montoTransferir;
	}else if(arregloCuentas[cuentaDestino] != 0) {
		montoTransferir = (rand() % (arregloCuentas[cuentaDestino]+1));
		printf("2. Monto a transferir: %d - Cuenta Origen: %d - Cuenta Destino %d \n",montoTransferir,cuentaDestino+1,cuentaOrigen+1);
		arregloCuentas[cuentaDestino] = arregloCuentas[cuentaDestino]-montoTransferir;
		arregloCuentas[cuentaOrigen] =  arregloCuentas[cuentaOrigen]+montoTransferir;
	}
    pthread_exit(0);
}

int main (int argc,char* argv[]) {	
	int parametros = argc -1;	
	
	if(parametros == 0) {
		printf("No se ingreso ningun parametro\n");
		return(0);
	}
	
	int nHilos = atoi(argv[1]);
	int cantidadTiempo = atoi(argv[2]);
 	int nCuentas = atoi(argv[3]);
	int valorInicial = atoi(argv[4]);  

	printf("Numero Hilos: %d Cantidad Tiempo: %d  Numero Cuentas: %d  Valor Inicial:  %d\n",nHilos,cantidadTiempo,nCuentas,valorInicial);	

	pthread_t arregloHilos[nHilos-1];
	arregloCuentas = malloc((nCuentas-1)*sizeof(int));
	int i;
	int j;
	for (i=0;i<nCuentas;i++) {
		arregloCuentas[i]=valorInicial;
	}
	
	srand(time(NULL));
	for (i=0;i<nHilos;i++) {
		int randOrigen = (rand() % (nCuentas));
		int randDest = (rand() % (nCuentas));
		while (randOrigen == randDest){
			randDest = (rand() % (nCuentas));
		}
		int origenYdestino[2]={randOrigen,randDest};
		for(j=0;j<cantidadTiempo;j++){
			pthread_create (&arregloHilos[i], NULL, (void *) transferencia, &origenYdestino);
		}
	}	
		
	for (i=0;i<nHilos;i++) {
		pthread_join (arregloHilos[i], NULL);
	}

	int balance=0;
	for (i=0;i<nCuentas;i++) {
		balance = balance + arregloCuentas[i];
		printf("Cuenta %d - Saldo %d\n",i+1,arregloCuentas[i]);
	}
	
	printf("El Balance total fue: %d\n",balance);
	pthread_exit(NULL);
	return(0);
}
