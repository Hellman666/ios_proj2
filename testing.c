#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NUMBER_OF_CUSTOMERS 4
#define MAX_NUMBER_OF_THREADS 10

//počet zákazníků v tuto chvíli
int numberOfCustomers = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//počet volných míst v místnosti
int numberOfSeats = 4;

//potřebné semafory
sem_t customersSemaphore;
sem_t barberSemaphore;
sem_t barbersStool; // barberova stolice

//potřebné vlákna
pthread_t barber;
pthread_t customers[MAX_NUMBER_OF_THREADS];

//prototypy funkcí
void balk();
void getHairCut();
void cutHair();
void *barberFunc(void *arg);
void *customersFunc(void *arg);

void balk() {
    printf("\nThe barber is busy, customer leaves the shop\n");
    pthread_mutex_unlock(&mutex);
}

void cutHair() {
    int timeNeededForAtrim = rand() % 6 + 1;

    sem_wait(&barbersStool);
    numberOfCustomers--;
    sem_post(&barbersStool);

    printf("\nThe customer is getting a trim which lasts %d minute(s)\n", timeNeededForAtrim);
    sleep(timeNeededForAtrim);
}

void getHairCut(){
    printf("The customer got his new Haircut, and is leaving the shop\n");
}

void *barberFunc(void *arg){
    while (1) {
    sem_wait(&customersSemaphore); // čeká, až přijde zákazník
    pthread_mutex_lock(&mutex);
    numberOfSeats++;
    balk(); // pokud nejsou volná místa, zákazník odejde
    sem_post(&barberSemaphore); // probudí zákazníka
    pthread_mutex_unlock(&mutex);
    cutHair(); // stříhání vlasů
    }
    return NULL;
}

void *customersFunc(void *arg)
{
    intptr_t customerId = (intptr_t)arg;
    sleep(rand() % 5 + 1); // náhodná doba před příchodem zákazníka
    pthread_mutex_lock(&mutex);
    if (numberOfSeats > 0) { // pokud jsou místa volná
        numberOfSeats--;
        numberOfCustomers++;
        printf("\nCustomer %ld enters the shop and sits down. Free seats: %d\n", customerId, numberOfSeats);
        sem_post(&customersSemaphore); // probudí holiče
        pthread_mutex_unlock(&mutex);
        sem_wait(&barberSemaphore); // čeká na holiče
        getHairCut(); // stříhání vlasů
    }
    else { // pokud nejsou místa volná
        printf("\nNo free seats. Customer %ld leaves the shop.\n", customerId);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main()
{
    srand(time(NULL)); // inicializace generátoru náhodných čísel
    // inicializace semaforů
    sem_init(&customersSemaphore, 0, 0);
    sem_init(&barberSemaphore, 0, 0);
    sem_init(&barbersStool, 0, 1); // barberova stolička je volná

    // vytvoření vlákna pro holiče
    pthread_create(&barber, NULL, barberFunc, NULL);

    // vytvoření vláken pro zákazníky
    int customerIds[MAX_NUMBER_OF_THREADS];
    for (int i = 0; i < MAX_NUMBER_OF_THREADS; i++) {
        customerIds[i] = i+1;
        pthread_create(&customers[i], NULL, customersFunc, &customerIds[i]);
    }

    // čekání na dokončení všech vláken
    pthread_join(barber, NULL);
    for (int i = 0; i < MAX_NUMBER_OF_THREADS; i++) {
        pthread_join(customers[i], NULL);
    }

    // zničení semaforů
    sem_destroy(&customersSemaphore);
    sem_destroy(&barberSemaphore);
    sem_destroy(&barbersStool);

    // zničení mutexu
    pthread_mutex_destroy(&mutex);

    return 0;
}