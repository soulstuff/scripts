/*
 * Problema del consumidor y productor
 * Este problema fue escrito mirando la documentación en Wikipedia 
 * como referencia para la clase de ayudantía de Sistemas Operativos
 * de ingeniería en ejecución informática de la PUCV con fines
 * educativos.
 *
 * José Toro
 *
 */
 

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<time.h> //Utilizado para los numeros aleatorios


#define PRODUCTORES 5 
#define CONSUMIDORES 3
#define BUFFER 5

/*
 * ESTRUCTURA SIMPLE DE PILA PARA DEMOSTRAR EL FUNCIONAMIENTO DE LAS HEBRAS
 */

typedef struct Pila{
	int tope;
	unsigned int capacidad;
	int* array;
} Pila;

Pila* crearPila(unsigned int capacidad){
	/*
	 * Crea una pila de números entero de tamaño [[capacidad]]
	 */
	Pila* tmp = (struct Pila*) malloc(sizeof(Pila));
	tmp->capacidad = capacidad;
	tmp->tope = -1;
	tmp->array = (int*) malloc(tmp->capacidad*sizeof(int));
	return tmp;
}

void push(Pila* pila, int objeto){
	/*
	 * Agrega un objeto a la pila
	 *
	 * args:
	 * (Pila) pila: puntero con la dirección de la pila donde agregaremos el objeto
	 * (int) objeto: objeto a agregar al tope de la pila
	 *
	 */
	pila->array[++pila->tope] = objeto;
	printf("Objeto agregado exitosamente!\n");
}

int pop(Pila* pila){
	/*
	 * Retorna el objeto en el tope de la pila
	 *
	 * args:
	 * (Pila) pila: puntero con la dirección de la pila
	 */
	return pila->array[pila->tope--];
}





//Creamos las listas de productores y consumidores
pthread_t h_productores[PRODUCTORES];
pthread_t h_consumidores[CONSUMIDORES];
pthread_t h_mostrar;
//Creamos el semaforo de espacio disponible, espacio utilizado y mutex
sem_t mutex;
sem_t utilizado;
sem_t disponible;
//Creamos una pila para la demostración de las hebras
Pila* objetos;

void* productor(void *id){
	/* Funcion del consumidor */
	unsigned int objeto;
	unsigned int id_hebra = (int)id;
	while(1){
		//Revisamos si podemos agregar cosas al buffer intentando bajar el semáforo
		sem_wait(&disponible);
		//Se produce un objeto(esperamos tiempo para que no sea instantaneo)
		objeto = rand()%50;
		printf("El productor %d ha producido el objeto %d\n", id_hebra, objeto);
		sleep(3);
		//Pedimos permiso para modificar el buffer
		sem_wait(&mutex);
		//ponemos el objeto en el buffer
		//(esperamos un tiempo para que no se instantaneo)
		sleep(2);
		printf("El productor %d intentará agregar el objeto %d a la pila\n", id_hebra, objeto);
		push(objetos, objeto);
		//liberamos el recurso
		sem_post(&mutex);
		//Anunciamos que hay un nuevo producto en el buffer
		sem_post(&utilizado);
	}
	
}	

void* consumidor(void *id){
	unsigned int id_hebra = (int)id;
	unsigned int objeto;
	while(1){
		//Revisamos si hay objectos disponibles en el buffer
		sem_wait(&utilizado);
		//Pedimos permiso para modificar el buffer
		sem_wait(&mutex);
		//Retiramos el objeto del buffer
		objeto = pop(objetos);
		sleep(3);

		printf("El consumidor %d ha retirado el objeto %d\n", id_hebra, objeto);
		//liberamos el recurso
		sem_post(&mutex);
		//Anunciamos que se ha liberado un espacio del buffer
		sem_post(&disponible);
	}
}

void* mostrarPila(void *arg){
	/*
	 * Muestra los objetos actuales en la pila
	 */
	while(1){
	sem_wait(&mutex);
	printf("La pila contiene actualmente:\n");
	if(objetos->tope < 0){
		printf("Nada, esta vacia\n");
	}else{
	for(int i=objetos->tope; i>=0; i--){
		printf("|%d|\n", objetos->array[i]);
	}
	printf("---\n");
	}
	sem_post(&mutex);
	sleep(1);
	}

}

int main(){
	//Variables de estatus
	int rc; //error de hebras
	void *status; //estado de hebras. No lo usamos en este ejemplo
	srand(time(NULL));
    objetos = crearPila(BUFFER);
	//inicializamos los semáforos
	sem_init(&mutex, 0, 1);
	sem_init(&utilizado, 0, 0);
	sem_init(&disponible, 0, BUFFER);

	//Inicializamos las hebras
	for(int i=0; i<CONSUMIDORES; i++){
		printf("Creando consumidor...\n");
		rc = pthread_create(&h_consumidores[i], NULL, consumidor, (void *)i);
		if(rc){
			printf("ERROR: Detectado error %d dentro de la creación del consumidor %d ", rc, i);
			exit(0);
		}
	}
	for(int i=0; i<PRODUCTORES; i++){
		printf("Creando productor...\n");
		rc = pthread_create(&h_productores[i], NULL, productor, (void *)i);
		if(rc){
			printf("ERROR: Detectado error %d dentro de la creación del productor %d", rc, i);
			exit(0);
		}
	}
	
	pthread_create(&h_mostrar, NULL, mostrarPila, NULL);
	if(rc){
		printf("ERROR: Detectado error %d al crear hebra de información", rc);
		exit(0);
	}
	//Unimos las hebras al main para que el programa espere que todas terminen
	//SPOILER: No van a terminar nunca
	
	for(int i=0; i<CONSUMIDORES; i++){
		rc = pthread_join(h_consumidores[i], &status);
		if (rc){
			printf("ERROR: Detectado error %d al unir consumidor %d", rc, i);
			exit(0);
		}
	}
	for(int i=0; i<PRODUCTORES; i++){
		rc = pthread_join(h_productores[i], &status);
		if(rc){
			printf("ERROR: Detectado error %d al unir productor %d", rc, i);
			exit(0);
		}
	}
	rc = pthread_join(h_mostrar, NULL);
	if(rc){
		printf("ERROR: Detectado error %d al unir hebra de información", rc);
		exit(0);
	}
	return 0;
}
