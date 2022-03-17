#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>//per sleep()
//per i semafori
#include <semaphore.h>//Per le funzioni sem_init(), sem_post() e sem_wait()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


void* cucinare();
void* selvaggiare();


//Semafori
pthread_mutex_t mutex; //Semaforo che gestisce l'accesso alla variabile porzioni
sem_t s_cuoco;
sem_t s_selvaggio;

//Variabili condivise
int max_porzioni;//Dimensione della pentola
int selvaggi;
int porzioni;//Porzioni nella pentola
int ngiri;//Numero di porzioni che ogni selvaggio mangia per volta


void* cucinare(){
     while(1){
        sem_wait(&s_cuoco);//down(s_cuoco, 1);//io cuoco ho bisogno di accedere a porzioni
        porzioni = max_porzioni;//maxporzioni è una variabile globale
        printf("Cuoco cucina tutte le porzioni, porzioni = %d\n", porzioni);
        sem_post(&s_selvaggio);//up(s_selvaggio, 1);
    }
}

void* selvaggiare(void* id){
    int i = *((int*)id);
    int ngiriperselvaggio = ngiri;
    while(ngiriperselvaggio > 0){//ogni selvaggio vuole mangiare ngiri volte
        sleep(1);
        pthread_mutex_lock(&mutex);//down(mutex)
        if(porzioni > 0){
            porzioni--;
            printf("Selvaggio %d mangia una porzione, porzioni = %d\n", i, porzioni);
            ngiriperselvaggio--;
        }
        else{
            printf("Porzioni finite\n"); 
            sem_post(&s_cuoco);//up(s_cuoco, 1);
            sem_wait(&s_selvaggio);//down(s_selvaggio, 1);
        }
        pthread_mutex_unlock(&mutex);//up(mutex)
    }
}

//Da input la quantità di selvaggi, la dimensione della pentola e ngiri
void main(int argc, char *argv[]){
    
    if(argv[1] != NULL && argv[2] != NULL && argv[3] != NULL && argc == 4){
        if(atoi(argv[1]) > 0 && atoi(argv[2]) > 0 && atoi(argv[3]) >= 0){
            selvaggi = atoi(argv[1]);
            max_porzioni = atoi(argv[2]);
            ngiri = atoi(argv[3]);
            printf("Porzioni = %d\n", max_porzioni);
            printf("Selvaggi = %d\n", selvaggi);
            printf("Giri per selvaggio = %d\n", ngiri);
            
            int i;
            int* a = (int*)malloc(selvaggi*sizeof(int));//Putatore a zona di memoria che contiene interi che rappresentano i selvaggi
            pthread_t cuoco;
            pthread_t* selvaggio = (pthread_t*)malloc(selvaggi*sizeof(pthread_t));
            pthread_mutex_init(&mutex, NULL);
            sem_init(&s_cuoco, 0, 1);//Inizializiamo il semaforo scuoco, indicando che sarà condiviso tra thread di questo processo e lo inizializziamo ad 1 (verde)
            sem_init(&s_selvaggio, 0, 1);
            
            //Come passo una funzione con più argomenti alla pthread_create()??????????
            pthread_create(&cuoco, NULL, cucinare, NULL);//crea un thread cuoco che usa gli attributi di default ed esegue inizialmente la funzione cucinare che non prende niente in ingresso
            
            for(i = 0; i < selvaggi; i++){
                a[i] = i;
                pthread_create(&selvaggio[i], NULL, selvaggiare, (void*)&a[i]);  
            }
            
            for(i = 0; i < selvaggi; i++)
                pthread_join(selvaggio[i], NULL);
            
            pthread_mutex_destroy(&mutex);//Distruggiamo il semaforo mutex
        }
    }
    
    
    /*int i;
    int* a = (int*)malloc(selvaggi*sizeof(int));//Putatore a zona di memoria che contiene interi che rappresentano i selvaggi
    pthread_t cuoco;
    pthread_t* selvaggio = (pthread_t*)malloc(selvaggi*sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);
    sem_init(&s_cuoco, 0, 1);//Inizializiamo il semaforo scuoco, indicando che sarà condiviso tra thread di questo processo e lo inizializziamo ad 1 (verde)
    sem_init(&s_selvaggio, 0, 1);
    
    //Come passo una funzione con più argomenti alla pthread_create()??????????
    pthread_create(&cuoco, NULL, cucinare, NULL);//crea un thread cuoco che usa gli attributi di default ed esegue inizialmente la funzione cucinare che non prende niente in ingresso
    
    for(i = 0; i < selvaggi; i++){
        a[i] = i;
        pthread_create(&selvaggio[i], NULL, selvaggiare, (void*)&a[i]);  
    }
    
    for(i = 0; i < selvaggi; i++)
        pthread_join(selvaggio[i], NULL);
    
    pthread_mutex_destroy(&mutex);//Distruggiamo il semaforo mutex
    */
}
