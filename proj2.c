#include "proj2.h"

sem_t *postal_sem;        
sem_t *counter;
sem_t *queue_one;
sem_t *queue_two;
sem_t *queue_three;
sem_t *officer_sem;
sem_t *customer_sem;

int *cislo_radku;
int NZ, NU, TZ, TU, F;
bool *is_postal_open;   
FILE *file;
int *queue_one_counter;
int *queue_two_counter;
int *queue_three_counter;

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

    postal_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    counter = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    cislo_radku = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    is_postal_open = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_one = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_two = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_three = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_one_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_two_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    queue_three_counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    officer_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    customer_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    
    sem_init(postal_sem, 1, 1);
    sem_init(counter, 1, 1);
    sem_init(queue_one, 1, 0);
    sem_init(queue_two, 1, 0);
    sem_init(queue_three, 1, 0);
    sem_init(officer_sem, 1, 0);
    sem_init(customer_sem, 1, 0);

    *queue_one_counter = 0;
    *queue_two_counter = 0;
    *queue_three_counter = 0;

    NZ = atoi(argv[1]);  
    NU = atoi(argv[2]);  
    TZ = atoi(argv[3]);  
    TU = atoi(argv[4]);  
    F = atoi(argv[5]);

    if(input_check(argc, argv) == 1) {
        return 1;
    }

    file = fopen("proj2.out", "w");
    *is_postal_open = true;
    create_process(NU, 1); //úředník
    create_process(NZ, 0); //zákazník
    
    int wait_time = ((rand() % (F/2 + 1)) + F/2);
    usleep(wait_time*1000);
    printf("%d \n", wait_time * 1000);
    *is_postal_open = false;
    my_print("closing\n");

    while(wait(NULL) > 0);
    clear();
    printf("Načtené argumenty:\nNZ: %d\nNU: %d\nTZ: %d\nTU: %d\nF: %d\n", NZ, NU, TZ, TU, F);

    return 0;
}

void officer(int idU, int TU){
    srand(time(NULL) + idU);
    my_print("U %d: started\n", idU);
    
    while((*is_postal_open) == true){
        if((*queue_one_counter) > 0 || (*queue_two_counter) > 0 || (*queue_three_counter) > 0) {
            printf("\npostal: %d, one: %d, two: %d, three: %d\n", *is_postal_open, *queue_one_counter,  *queue_two_counter, *queue_three_counter);
            int service = (rand() % 3) + 1;

            if(service == 1 && (*queue_one_counter == 0)){
                service = 2;
            }
            if(service == 2 && (*queue_two_counter == 0)){
                service = 3;
            }
            if(service == 3 && (*queue_three_counter == 0)){
                service = 1;
            }

            if(service == 1 && (*queue_one_counter > 0)){
                my_print("U %d: serving a service of type %d\n", idU, service);
                sem_post(queue_one);
                usleep((rand() % 11) * 1000);
                my_print("U %d: service finished\n", idU);
                //sem_wait(queue_one);
            }
            
            if(service == 2 && (*queue_two_counter > 0)){
                my_print("U %d: serving a service of type %d\n", idU, service);
                sem_post(queue_two);
                usleep((rand() % 11) * 1000);
                my_print("U %d: service finished\n", idU);
                //sem_wait(queue_two);
            }

            if(service == 3 && (*queue_three_counter > 0)){
                my_print("U %d: serving a service of type %d\n", idU, service);
                sem_post(queue_three);
                usleep((rand() % 11) * 1000);
                my_print("U %d: service finished\n", idU);
                //sem_wait(queue_three);
            }
        }

        if((*queue_one_counter == 0) && (*queue_two_counter == 0) && (*queue_three_counter == 0) && *is_postal_open == true){
            my_print("U %d: taking break\n", idU);
            usleep((rand() % (TU + 1))*1000);
            my_print("U %d: break finished\n", idU);
        }
    }

    sem_wait(postal_sem);
    if((*is_postal_open) == false){
        my_print("U %d: going home\n", idU);
    }
    sem_post(postal_sem);
}

void customer(int idZ, int TZ){
    srand(time(NULL) + idZ);
    my_print("Z %d: started\n", idZ);
    usleep((rand() % (TZ + 1)) * 1000);

    if(*is_postal_open == false){
        my_print("Z %d: going home\n", idZ);
        exit(0);
    }

    int service = ((rand() % 3) + 1);
    my_print("Z %d: entering office for a service %d\n", idZ, service);
    printf("service zakaznik: %d\n", service);
    if (service == 1) {
        
        (*queue_one_counter)++;
        sem_wait(queue_one);
    }
    if (service == 2) {
        
        (*queue_two_counter)++;
        sem_wait(queue_two);
    }
    if (service == 3) {
    
        (*queue_three_counter)++;
        sem_wait(queue_three);
    }

    my_print("Z %d: called by office worker\n", idZ);

    if(service == 1){
        (*queue_one_counter)--;
        //sem_post(queue_one);
    }
    if(service == 2){
        (*queue_two_counter)--;
        //sem_post(queue_two);
    }
    if(service == 3){
        (*queue_three_counter)--;
        //sem_post(queue_three);
    }
    usleep((rand() %11) * 1000);
    my_print("Z %d: going home\n", idZ);
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
    sem_destroy(postal_sem);
    sem_destroy(counter);
    sem_destroy(queue_one);
    sem_destroy(queue_two);
    sem_destroy(queue_three);
    sem_destroy(customer_sem);
    sem_destroy(officer_sem);
    munmap(postal_sem, sizeof(sem_t));
    munmap(counter, sizeof(sem_t));
    munmap(queue_one, sizeof(sem_t));
    munmap(queue_two, sizeof(sem_t));
    munmap(queue_three, sizeof(sem_t));
    munmap(customer_sem, sizeof(sem_t));
    munmap(officer_sem, sizeof(sem_t));
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