#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>

//Variables globales
int *arregloCuentas;
sem_t *semaforos;
int cantidadTiempo;
int nCuentas;
//Metodo que sera ejecutado por cada hilo para realizar transferencias de una cuenta a otra
void* transferencia(void* args){
	int j=0;
	int randOrigen;
	int randDest;
	unsigned int seed = (unsigned) time(NULL) * rand();
	for(j=0;j<cantidadTiempo;j++){
		bool desocupados=true;//Variable para indicar si se esta realizando o no alguna transferencia
		srand(seed);//Inicializacion de numeros aleatorios
		randOrigen = (rand() % (nCuentas));//Generacion aleatoria de la cuenta de origen
		randDest = (rand() % (nCuentas));//Generacion aleatoria de la cuenta de destino
		while (randOrigen == randDest){//Mientras la cuenta de origen sea la misma cuenta de destino, se debe elegir una nueva cuenta de destino
			randDest = (rand() % (nCuentas));
		}
		while(desocupados){//Si no se esta realizando niguna transferencia, el hilo correspondiente tiene via libre para hacerla
			int semaforo_origen=sem_trywait(&semaforos[randOrigen]);//Se intenta bloquear el semaforo que corresponde a la cuenta de origen
			int semaforo_destino=sem_trywait(&semaforos[randDest]);//Se intenta bloquear el semaforo que corresponde a la cuenta de destino
			if(semaforo_origen == 0 && semaforo_destino == 0){//Si ambos semaforos se encuentran bloqueados, el hilo puede realizar sin problemas la transferencia
				int montoTransferir = 0;
				desocupados=false;
				if(arregloCuentas[randOrigen] != 0){//Si la cuenta de origen tiene saldo se realiza la trasferencia
					montoTransferir = (rand() % (arregloCuentas[randOrigen]+1));
					printf("1. Monto a transferir: %d - Cuenta Origen: %d - Cuenta Destino %d \n",montoTransferir,randOrigen+1,randDest+1);
					arregloCuentas[randOrigen] = arregloCuentas[randOrigen]-montoTransferir;				
					arregloCuentas[randDest] =  arregloCuentas[randDest]+montoTransferir;				
				}else if(arregloCuentas[randDest] != 0) {//Si la cuenta de origen no tiene saldo pero la de destino si, la transferencia se realiza desde la cuenta de destino
					montoTransferir = (rand() % (arregloCuentas[randDest]+1));
					printf("2. Monto a transferir: %d - Cuenta Origen: %d - Cuenta Destino %d \n",montoTransferir,randDest+1,randOrigen+1);
					arregloCuentas[randDest] = arregloCuentas[randDest]-montoTransferir;
					arregloCuentas[randOrigen] =  arregloCuentas[randOrigen]+montoTransferir;
				}else{//Si ninguna de las dos cuentas tiene saldo, el hilo no realiza ninguna transaccion
					printf("3. Monto a transferir: %d - Cuenta Origen: %d - Cuenta Destino %d \n",0,randDest+1,randOrigen+1);
				}			
			}else{
				if(semaforo_origen == 0){//Se libera el semaforo de la cuenta origen
					sem_post(&semaforos[randOrigen]);
				}else if(semaforo_destino == 0){//Se libera el semaforo de la cuenta destino
					sem_post(&semaforos[randDest]);
				}
			}
		}
		//Liberacion de los semaforos
		sem_post(&semaforos[randOrigen]);	
		sem_post(&semaforos[randDest]);
		seed = seed * (unsigned) time(NULL) + getpid();//Inicializacion de los numeros aleatorios
	}
	return NULL;
}

int main (int argc,char* argv[]) {	
	int parametros = argc -1;	
	
	if(parametros == 0) {
		printf("No se ingreso ningun parametro\n");
		return(0);
	}
	
	int nHilos = atoi(argv[1]);
	cantidadTiempo = atoi(argv[2]);
 	nCuentas = atoi(argv[3]);
	int valorInicial = atoi(argv[4]);  

	printf("Numero Hilos: %d Cantidad Tiempo: %d  Numero Cuentas: %d  Valor Inicial:  %d\n",nHilos,cantidadTiempo,nCuentas,valorInicial);	

	pthread_t arregloHilos[nHilos-1];
	arregloCuentas = malloc(sizeof(int) * (nCuentas)+1);//Se reserva espacio en memoria para el arreglo de cuentas
	semaforos=malloc(sizeof(sem_t) * (nCuentas));//Se reserva espacio en memoria para el arreglo de semaforos
	int i;
	
	for(i=0;i<nCuentas;i++) {
		arregloCuentas[i]=valorInicial;//Se inicilizan todas las cuentas segun el valor pasado como parametro
		sem_init(&semaforos[i],0,1);	//Se inicializan los semaforos			
	}
 	for(i=0;i<nHilos;i++) {
		pthread_create (&arregloHilos[i], NULL, (void *)transferencia, NULL);//Creacion concurrente de hilos segun la cantidad indicada
	}	
		
	for (i=0;i<nHilos;i++) {
		pthread_join (arregloHilos[i], NULL);//Espera por la terminacion de la tarea de cada uno de los hilos
	}

	for (i=0;i<nCuentas;i++) {
		sem_destroy(&semaforos[i]);//Destruccion de los semaforos, que ya no se usaran mas	
	}

	int balance=0;
	//calculo del balace del saldo de las cuentas
	for (i=0;i<nCuentas;i++) {
		balance = balance + arregloCuentas[i];
		printf("Cuenta %d - Saldo %d\n",i+1,arregloCuentas[i]);
	}
	
	printf("El Balance total fue: %d\n",balance);
	exit(0);
}
