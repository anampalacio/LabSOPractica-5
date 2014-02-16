#include <pthread.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <semaphore.h>
 #include <stdbool.h>
 #include <unistd.h>
 
//VARIABLES GLOBALES
 int *arregloCuentas;
 sem_t *semaforos;
 int cantidadTiempo;
 int nCuentas;
 
void* transferencia(void* args){
         int j=0;
         int randOrigen;
         int randDest;
         unsigned int seed = (unsigned) time(NULL) * rand();
         for(j=0;j<cantidadTiempo;j++){
                 bool desocupados=true;
                 srand(seed);
                 randOrigen = (rand() % (nCuentas));
                 randDest = (rand() % (nCuentas));
                 while (randOrigen == randDest){
                         randDest = (rand() % (nCuentas));
                 }
                 while(desocupados){
                         int semaforo_origen=sem_trywait(&semaforos[randOrigen]);
                         int semaforo_destino=sem_trywait(&semaforos[randDest]);
                         if(semaforo_origen == 0 && semaforo_destino == 0){
                                 int montoTransferir = 0;
                                 desocupados=false;
                                 if(arregloCuentas[randOrigen] != 0){
                                         montoTransferir = (rand() % (arregloCuentas[randOrigen]+1));
                                         printf("1. Monto a transferir: %d - Cuenta Origen: %d - Cuenta
 Destino %d \n",montoTransferir,randOrigen+1,randDest+1);
                                         arregloCuentas[randOrigen] =
 arregloCuentas[randOrigen]-montoTransferir;
                                         arregloCuentas[randDest] =  arregloCuentas[randDest]+montoTransferir;
                                 }else if(arregloCuentas[randDest] != 0) {
                                         montoTransferir = (rand() % (arregloCuentas[randDest]+1));
                                         printf("2. Monto a transferir: %d - Cuenta Origen: %d - Cuenta
 Destino %d \n",montoTransferir,randDest+1,randOrigen+1);
                                         arregloCuentas[randDest] = arregloCuentas[randDest]-montoTransferir;
                                         arregloCuentas[randOrigen] =  arregloCuentas[randOrigen]+montoTransferir;
                                 }else{
                                         printf("3. Monto a transferir: %d - Cuenta Origen: %d - Cuenta
 Destino %d \n",0,randDest+1,randOrigen+1);
                                 }
                         }else{
                                 if(semaforo_origen == 0){
                                         sem_post(&semaforos[randOrigen]);
                                 }else if(semaforo_destino == 0){
                                         sem_post(&semaforos[randDest]);
                                 }
                         }
                 }
                 sem_post(&semaforos[randOrigen]);
                 sem_post(&semaforos[randDest]);
                 seed = seed * (unsigned) time(NULL) + getpid();
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
 
        printf("Numero Hilos: %d Cantidad Tiempo: %d  Numero Cuentas: %d
 Valor Inicial:  %d\n",nHilos,cantidadTiempo,nCuentas,valorInicial);
 
        pthread_t arregloHilos[nHilos-1];
         arregloCuentas = malloc(sizeof(int) * (nCuentas)+1);
         semaforos=malloc(sizeof(sem_t) * (nCuentas));
         int i;
 
        for(i=0;i<nCuentas;i++) {
                 arregloCuentas[i]=valorInicial;
                 sem_init(&semaforos[i],0,1);
         }
         for(i=0;i<nHilos;i++) {
                 pthread_create (&arregloHilos[i], NULL, (void *)transferencia, NULL);
         }
 
        for (i=0;i<nHilos;i++) {
                 pthread_join (arregloHilos[i], NULL);
         }
 
        for (i=0;i<nCuentas;i++) {
                 sem_destroy(&semaforos[i]);
         }
 
        int balance=0;
 
        for (i=0;i<nCuentas;i++) {
                 balance = balance + arregloCuentas[i];
                 printf("Cuenta %d - Saldo %d\n",i+1,arregloCuentas[i]);
         }
 
        printf("El Balance total fue: %d\n",balance);
         exit(0);
 }
