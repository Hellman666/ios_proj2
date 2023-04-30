#include "proj2.h"

sem_t *postal_sem;        
sem_t *counter;
sem_t *queue_one;
sem_t *queue_two;
sem_t *queue_three;

FILE *file;
bool *is_postal_open; 
int *cislo_radku;
int NZ, NU, TZ, TU, F;
int *queue_one_counter;
int *queue_two_counter;
int *queue_three_counter;

// vytvoří se funkce my_print, která použije po každém printu ffslush
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

int main(int argc, char *argv[])
{
    srand(time(NULL));

    
    // Vytvoření sdílených proměnných a inicializace semaforů
    postal_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // semafor pro kritickou sekci přístupu k poště
    counter = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // semafor pro kritickou sekci přístupu ke sdílenému čítači
    cislo_radku = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // číslo aktuálního řádku v souboru
    is_postal_open = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // informace, zda je pošta otevřená
    queue_one = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // semafor pro frontu zákazníků 1
    queue_two = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // semafor pro frontu zákazníků 2
    queue_three = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // semafor pro frontu zákazníků 3
    queue_one_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // počet zákazníků ve frontě 1
    queue_two_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // počet zákazníků ve frontě 2
    queue_three_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0); // počet zákazníků ve frontě 3

    sem_init(postal_sem, 1, 1); // inicializace semaforu pro kritickou sekci přístupu k poště
    sem_init(counter, 1, 1); // inicializace semaforu pro kritickou sekci přístupu ke sdílenému čítači
    sem_init(queue_one, 1, 0); // inicializace semaforu pro frontu zákazníků 1
    sem_init(queue_two, 1, 0); // inicializace semaforu pro frontu zákazníků 2
    sem_init(queue_three, 1, 0); // inicializace semaforu pro frontu zákazníků 3

    *queue_one_counter = 0; // inicializace počtu zákazníků ve frontě 1 na hodnotu 0
    *queue_two_counter = 0; // inicializace počtu zákazníků ve frontě 2 na hodnotu 0
    *queue_three_counter = 0; // inicializace počtu zákazníků ve frontě 3 na hodnotu 0

    NZ = atoi(argv[1]);  // počet zákazníků
    NU = atoi(argv[2]);  // počet úředníků   
    TZ = atoi(argv[3]);  // maximální čas, po který zákazník po svém vytvoření čeká, než vejde na poštu
    TU = atoi(argv[4]);  // maximální délka přestávky úředníka
    F = atoi(argv[5]);   // maximální čas, po kterém je uzavřena pošta pro nově příchozí.

    //kontrola, jestli jsou správné vstupy
    if(input_check(argc, argv) == 1) {
        return 1;
    }
    
    file = fopen("proj2.out", "w");
    *is_postal_open = true; //otevření pošty
    create_process(NU, 1); //vytvoření procesu úředník
    create_process(NZ, 0); //vytvoření procesu zákazník
    

    int wait_time = ((rand() % (F/2 + 1)) + F/2);
    usleep(wait_time*1000);
    *is_postal_open = false; // uzavření pošty po uplinutí času <F/2, F>
    
    my_print("closing\n");

    while(wait(NULL) > 0);
    clear();

    return 0;
}

// funkce officer, která zajišťuje přístupy úředníků ke zákazníkům
void officer(int idU, int TU){
    srand(time(NULL) + idU);
    my_print("U %d: started\n", idU);
    
    
    while((*is_postal_open) == true){
        // pokud je pošta otevřená  a v řadě jsou lidé
        if((*queue_one_counter) > 0 || (*queue_two_counter) > 0 || (*queue_three_counter) > 0) {
            int service = (rand() % 3) + 1;

            // vybere neprázdnou řadu
            if(service == 1 && (*queue_one_counter == 0)){
                service = 2;
            }
            if(service == 2 && (*queue_two_counter == 0)){
                service = 3;
            }
            if(service == 3 && (*queue_three_counter == 0)){
                service = 1;
            }

            // jakmile vybere řadu, která je neprázdná, začne provádět službu
            if(service == 1 && (*queue_one_counter > 0)){
                my_print("U %d: serving a service of type %d\n", idU, service);
                sem_post(queue_one);
                usleep((rand() % 11) * 1000);
                my_print("U %d: service finished\n", idU);
            }
            
            if(service == 2 && (*queue_two_counter > 0)){
                my_print("U %d: serving a service of type %d\n", idU, service);
                sem_post(queue_two);
                usleep((rand() % 11) * 1000);
                my_print("U %d: service finished\n", idU);
            }

            if(service == 3 && (*queue_three_counter > 0)){
                my_print("U %d: serving a service of type %d\n", idU, service);
                sem_post(queue_three);
                usleep((rand() % 11) * 1000);
                my_print("U %d: service finished\n", idU);
            }
        }

        // pokud je pošta otevřená a fronty jsou prázdné, vezme si účetník pauzu
        if((*queue_one_counter == 0) && (*queue_two_counter == 0) && (*queue_three_counter == 0) && *is_postal_open == true){
            my_print("U %d: taking break\n", idU);
            usleep((rand() % (TU + 1))*1000);
            my_print("U %d: break finished\n", idU);
        }

        // pokud je pošta zavřená a v žádné frontě není zákazník, účetník jde dom
        if((*is_postal_open) == false && (*queue_one_counter == 0) && (*queue_two_counter == 0) && (*queue_three_counter == 0)){
            my_print("U %d: going home\n", idU);
        }
    }
}

// funkce customer, ve které se provádí chody zákazníka na poště4
void customer(int idZ, int TZ){
    srand(time(NULL) + idZ);
    my_print("Z %d: started\n", idZ);
    usleep((rand() % (TZ + 1)) * 1000);
    
    // pokud je pošta zavřená, zákazník jde dom
    if(*is_postal_open == false){
        my_print("Z %d: going home\n", idZ);
        exit(0);
    }

    // zákazník si vybere číslo služby
    int service = ((rand() % 3) + 1);
    my_print("Z %d: entering office for a service %d\n", idZ, service);
    if (service == 1) {
        // zákazník se zařadí do fronty
        (*queue_one_counter)++;
        sem_wait(queue_one);
    }
    if (service == 2) {
        // zákazník se zařadí do fronty
        (*queue_two_counter)++;
        sem_wait(queue_two);
    }
    if (service == 3) {
        // zákazník se zařadí do fronty
        (*queue_three_counter)++;
        sem_wait(queue_three);
    }

    // zavolá se zákazník úředníkem
    my_print("Z %d: called by office worker\n", idZ);

    // po zavolání se zákazník vytáhne z fronty
    if(service == 1){
        (*queue_one_counter)--;
    }
    if(service == 2){
        (*queue_two_counter)--;
    }
    if(service == 3){
        (*queue_three_counter)--;
    }
    // zákazník jde domů
    usleep((rand() %11) * 1000);
    my_print("Z %d: going home\n", idZ);
}

// funkce na vytvoření procesů zákazníků a úředníků
void create_process(int processCount, int processType) {
    for(int i = 1; i < processCount+1; i++) {
        pid_t pid = fork();
        if(pid == -1) {
            fprintf(stderr, "Chyba: nepodařilo se vytvořit proces\n");
            exit(1);
        } else if (pid == 0) {
            if(processType == 0) {
                customer(i, TZ);
                exit(0);
              
            }  else {
                officer(i, TU);
                exit(0);
            }   
        }
    }
}

// funkce na vyčištění paměti
void clear() {
    sem_destroy(postal_sem);
    sem_destroy(counter);
    sem_destroy(queue_one);
    sem_destroy(queue_two);
    sem_destroy(queue_three);
    munmap(postal_sem, sizeof(sem_t));
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

// funkce na kontrolu vstupů
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