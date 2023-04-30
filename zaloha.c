#include "proj2.h"

//testy:
// https://github.com/nukusumus/IOS
// https://github.com/matusHubinsky/IOS-2-project-deadlock-tester
// https://github.com/Blacki005/IOS_tester_2023

//Zadání:
//https://moodle.vut.cz/pluginfile.php/577383/mod_resource/content/1/zadani-2023.pdf

//sem_t *mutex;
//mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
//mmap = sdílená paměť
//unmap - alternativa na čištění paměti
//munmap() - vytvoří podprocesy?
//sem_init(mutex, 1, 1); 
//sem_destroy(mutex);
//semafory a sdílená paměť se dělá jednou a bude pro všechny procesy
//!setbuf(file, NULL); / fflush()


//budu chtít volat v každém dítěti:
//srand pro kazdy proces aby měl random cisla
//usleep po vytvoreni
//usleep(1000 * (rand() % (TI + 1)));

// opisu z knizky





sem_t *mutex;       //todo: můžu smazat
sem_t *customers;   //todo: můžu smazat
sem_t *officers;    //todo: můžu smazat
sem_t *counter;
sem_t *queue_one;
sem_t *queue_two;
sem_t *queue_three;

int *cislo_radku;
int NZ, NU, TZ, TU, F;          /// vytovření proměnných pro vstupy
bool *is_postal_open;    //  vytvoření proměnné, jestli je pošta open
FILE *file;
int *queue_one_counter;
int *queue_two_counter;
int *queue_three_counter;

bool queue;


void my_print(const char * format, ...){
    sem_wait(counter);
    va_list args;
    va_start (args, format);
    fprintf(file, "%d: ", (*cislo_radku)+1);
    (*cislo_radku)++;
    vfprintf (file, format, args);
    fflush(file);
    va_end(args);
    sem_post(counter);
}

int main(int argc, char *argv[]) {

    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    customers = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    officers = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    counter = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    cislo_radku = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    is_postal_open = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_one = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_two = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_three = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_one_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_two_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_three_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    

    sem_init(mutex, 1, 1);
    sem_init(customers, 1, 0);
    sem_init(officers, 1, 0);
    sem_init(counter, 1, 1);
    sem_init(queue_one, 1, 1);
    sem_init(queue_two, 1, 1);
    sem_init(queue_three, 1, 1);

    srand(time(NULL));

    //přiřazení vstupů k proměnným
    NZ = atoi(argv[1]);  //počet zákazníků
    NU = atoi(argv[2]);  //počet úředníků
    TZ = atoi(argv[3]);  // Maximální čas, který zákazní čeká, než vejde na poštu
    TU = atoi(argv[4]);  // Maximální délka přestávky úředníka
    F = atoi(argv[5]);   // Maximální čas, po kterém je pošta uzavřena

    //pokud nejsou vstupy správné, ukončí program
    if(input_check(argc, argv) == 1) {
        return 1;
    }

    //otevření/vytvoření souboru proj2.out
    file = fopen("proj2.out", "w");
    
    *is_postal_open = true;

    // vytvoření procesů úředníků
    create_process(NU, 1);

    // vytvoření procesů zákazníků
    create_process(NZ, 0);
    
    

    

    // vygeneruje se náhodný čas od F/2 po F
    int wait_time = ((rand() % (F/2 + 1)) + F/2);
    usleep(wait_time * 1000);
    printf("%d \n", wait_time * 1000);     
    *is_postal_open = false;


    my_print("closing\n");
    printf("Procesy skončily\n");
    
    
       
    while(wait(NULL) > 0);
    clear(file);

    printf("Načtené argumenty:\n");
    printf("NZ: %d\nNU: %d\nTZ: %d\nTU: %d\nF: %d\n", NZ, NU, TZ, TU, F);


    return 0;
}

void customer(int idZ, int TZ) {  
    my_print("Z %d: started\n", idZ);
    usleep((rand() % (TZ + 1)) * 1000);
    
    if(*is_postal_open == false){
        my_print("Z %d: going home\n", idZ);
        exit(0);
    }

    queue = true;
    srand(getpid());
    int service = (rand() % 3) + 1;
    my_print("Z %d: entering office for a service %d\n", idZ, service);
    //printf("\nservice zakaznik: %d\n", service);
    my_print("Z %d: called by office worker\n", idZ);
    
    // vyčkání náhodné doby na dokončení žádosti
    
    usleep((rand() % (TZ + 1)) * 1000);

    my_print("Z %d: going home\n", idZ);
    queue = false;

}

void officer(int idU, int TU) {
    my_print("U %d: started\n", idU);
    srand(getpid());
    int service = (rand() % 3) + 1;
    my_print("U %d: serving a service of type %d\n", idU, service);
    printf("\nservice urednik: %d\n", service);
    usleep((rand() % 11) * 1000);
    my_print("U %d: service finished\n", idU);

    //pokud v žádné frontě nečeká zákazní a pošta je otevřená
    if(queue == false && *is_postal_open == true){
        my_print("U %d: taking break\n", idU);
        usleep((rand() % (TU + 1))*1000);
        my_print("U %d: break finished\n", idU);
    }
    
    if(*is_postal_open == false){
        my_print("U %d: going home\n", idU);
    }
}


void create_process(int processCount, int processType) {
    for(int i = 1; i < processCount+1; i++) {
        pid_t pid = fork();
        if(pid == -1) {
            fprintf(stderr, "Chyba: nepodařilo se vytvořit proces\n");
            exit(1);
        } else if (pid == 0) {
            if(processType == 0) {
                fprintf(stdout, "Proces zákazníka %d vytvořen\n", getpid());
                customer(i, TZ);
                exit(0);
              
            }  else {
                fprintf(stdout, "Proces úředníka %d vytvořen\n", getpid());
                officer(i, TU);
                exit(0);
            }   
        }
    }
}

void clear() {
    sem_destroy(mutex);
    sem_destroy(customers);
    sem_destroy(officers);
    sem_destroy(counter);
    sem_destroy(queue_one);
    sem_destroy(queue_two);
    sem_destroy(queue_three);
    munmap(mutex, sizeof(sem_t));
    munmap(customers, sizeof(sem_t));
    munmap(officers, sizeof(sem_t));
    munmap(counter, sizeof(sem_t));
    munmap(queue_one, sizeof(sem_t));
    munmap(queue_two, sizeof(sem_t));
    munmap(queue_three, sizeof(sem_t));
    munmap(cislo_radku, sizeof(int));
    munmap(is_postal_open, sizeof(bool));
    munmap(queue_one_counter, sizeof(int));
    munmap(queue_two_counter, sizeof(int));
    munmap(queue_three_counter, sizeof(int));
    fclose(file);
}

int input_check(int argc, char *argv[]) {  
    //Zabezpečení, jestli je správný počet argumentů
    if(argc != 6) {
        printf("Chybný počet načtených argumentů\n");
        return 1;
    }

    //Zabezpečení, aby každý argument byl číslo
    char *params[] = {"argument 1", "argument 2", "argument 3", "argument 4", "argument 5"};
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j] != '\0'; j++) {
            if (!isdigit(argv[i][j])) {
                fprintf(stderr, "Chyba: %s není číslo. \n", params[i-1]);
                return 1;
            }
        }
    }

    //Kontrola, jestli odpovídají vstupy rozsahům
    if(NZ < 0) {
        fprintf(stderr, "Počet zákazníků neodpovídá rozsahu 1 - NU");
        return 1;
    }
    if(NU < 0) {
        fprintf(stderr, "počet úředníů neodpovídá rozsahu 1 - NZ");
        return 1;
    }
    if(TZ > 10000 || TZ < 0) { 
        fprintf(stderr, "Délka času, který zákazník čeká je mimo rozsah 0 - 10000\n");
        return 1;
    }
    if(TU > 100 || TU < 0) {
        fprintf(stderr, "Délka přestávky úředníka je mimo rozsah 0 - 100\n");
        return 1;
    }
    if(F > 10000 || F < 0) {
        fprintf(stderr, "Délka pro uzavření pošty pro nově příchozí je mimo rozsah 0 - 10000\n");
        return 1;
    }
    return 0;
}