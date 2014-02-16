#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <wait.h>

int main (int argc, char *argv []){
	FILE * archivo;
	char *nombre_programa= argv[1];
	archivo = fopen (argv[2],"r");
	int numeroHilos,tiempoRecorrer,numeroCuentas,valorInicial,cantidadRepeticiones;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int res=0;
	int status=0;
	int vectorPrueba=1;
	if (archivo == NULL) {
		printf("No se pudo abrir el archivo");
		exit (0);
	}

	while ((read = getline(&line, &len, archivo)) != -1) {
		res = sscanf(line, "%d %d %d %d %d", &numeroHilos,&tiempoRecorrer,&numeroCuentas,&valorInicial,&cantidadRepeticiones);
		char numero_hilos[1024]; 
		char tiempo_a_correr[1024]; 
		char numero_cuentas[1024]; 
		char valor_inicial[1024];
		sprintf(numero_hilos,"%d",numeroHilos);
		sprintf(tiempo_a_correr,"%d",tiempoRecorrer);				 		
		sprintf(numero_cuentas,"%d",numeroCuentas);				 		
		sprintf(valor_inicial,"%d",valorInicial);				 					 		
		if(res == 5){
			int i;			
			for(i=0;i<cantidadRepeticiones;i++){				
				pid_t pid_nuevo = fork();
				if(pid_nuevo == 0){//Nos encontramos en el proceso hijo
					printf("Vector Prueba %d--> numeroHilos: %d tiempoRecorrer: %d numeroCuentas: %d valorInicial: %d cantidadRepeticiones: %d\n",vectorPrueba,numeroHilos,tiempoRecorrer,numeroCuentas,valorInicial,cantidadRepeticiones);					
					execl(nombre_programa,nombre_programa,numero_hilos,tiempo_a_correr,numero_cuentas,valor_inicial,NULL);				
					exit(0);				
				}
				else{//Nos encontramos en el proceso padre y debemos esperar la ejecucion del hijo
					wait(&status);
				}
			}	
		}else{
			printf("No se procesara el Vector de Prueba %d debido a que no cumple con la cantidad de parametros requeridos: %s",vectorPrueba,line);
		}
		vectorPrueba++;
	}

	if (line){
		free(line);
	}
	fclose(archivo);
	return 0;
}
