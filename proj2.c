#include "proj2.h"

//https://www.youtube.com/watch?v=mXo8wSXUKvo
//https://www.youtube.com/watch?v=6KXvK6el5R4
//https://moodle.vut.cz/pluginfile.php/577383/mod_resource/content/1/zadani-2023.pdf


int NZ, NU, TZ, TU, F;  /// vytovření proměnných pro vstupy
int counter = 1;        /// vytvoření counteru pro výstup   
bool is_postal_open = false;
sem_t *sem_queue_1;
sem_t *sem_queue_2;
sem_t *sem_queue_3;

int main(int argc, char *argv[]) {
    if(argc != 6) {
        printf("Chybný počet načtených argumentů\n");
        return 1;
    }

    //otevření souboru proj2.out
    FILE *file;
    file = fopen("proj2.out", "w");

    char *params[] = {"argument 1", "argument 2", "argument 3", "argument 4", "argument 5"};
    for (int i = 1; i < argc; i++) {
        for (int j = 0; argv[i][j] != '\0'; j++) {
            if (!isdigit(argv[i][j])) {
                fprintf(stderr, "Chyba: %s není číslo. \n", params[i-1]);
                return 1;
            }
        }
    }
    //přiřazení vstupů k proměnným
    NZ = atoi(argv[1]);  //počet zákazníků
    NU = atoi(argv[2]);  //počet úředníků
    TZ = atoi(argv[3]);  // Maximální čas, který zákazní čeká, než vejde na poštu
    TU = atoi(argv[4]);  // Maximální délka přestávky úředníka
    F = atoi(argv[5]);   // Maximální čas, po kterém je pošta uzavřena

    //pokud nejsou vstupy správné, ukončí program
    if(input_check(NZ, NU, TZ, TU, F) == 1) {
        return 1;
    }
    

    // vytvoření procesů zákazníků
    createProcesses(NZ, 0);
    
    // vytvoření procesů úředníků
    createProcesses(NU, 1);

    int lower_f = F/2;
    int wait_time = (rand() % (F - lower_f)) + lower_f;

    usleep(wait_time * 1000);
    printf("%d \n", wait_time * 1000);
    is_postal_open = true;

    for(int i = 0; i <= NZ; i++) {
      customer(file, i, TZ);
    }

    fprintf(file, "%d: closing\n", counter++);
    printf("Procesy skončily\n");
    is_postal_open = false;
    
    
    clear(file);   

    
    printf("Načtené argumenty:\n");
    printf("NZ: %d\nNU: %d\nTZ: %d\nTU: %d\nF: %d\n", NZ, NU, TZ, TU, F);


    return 0;
}

int input_check(int NZ, int NU, int TZ, int TU, int F) {  
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

void customer(FILE *file, int idZ, int TZ) {

   //vytvoření náhodného čísla od 1 do 3
   int x = random_number(1, 3, 1);   

   srand(time(NULL));
   int wait_time = rand() % (TZ +1);

   fprintf(file, "%d: Z %d: started\n", counter++, idZ);
   usleep(wait_time * 1000);
   
   
   //zjistí, jestli je pošta otevřena
   if(is_postal_open == true) {
      fprintf(file, "%d: Z %d: entering office for a service %d\n", counter++, idZ, x);
      fprintf(file, "%d: Z %d: called by office worker\n", counter++, idZ);
      fprintf(file, "%d: Z %d: going home\n", counter++, idZ); 
   } else {
      fprintf(file, "%d: Z %d: going home\n", counter++, idZ); 
   }
}

void officer(FILE *file, int idU, int TU) {

    // Create semaphores
    sem_queue_1 = sem_open("/sem_queue_1", O_CREAT, 0666, 0);
    sem_queue_2 = sem_open("/sem_queue_2", O_CREAT, 0666, 0);
    sem_queue_3 = sem_open("/sem_queue_3", O_CREAT, 0666, 0);

    

    // Use the semaphores
    sem_wait(sem_queue_1); // Wait on semaphore for queue 1
    sem_post(sem_queue_2); // Signal semaphore for queue 2


    //vytvoření náhodného čísla od 1 do 3
    int queue_number = random_number(1, 3, 1);   
    switch(queue_number) {
        case 1:
            // přiřadit úředníka ke frontě 1
            sem_wait(&sem_queue_1);
            break;
        case 2:
            // přiřadit úředníka ke frontě 2
            sem_wait(&sem_queue_2);
            break;
        case 3:
            // přiřadit úředníka ke frontě 3
            sem_wait(&sem_queue_3);
            break;
        default:
            // nepřiřazeno, nastala chyba
            break;
    }

    srand(time(NULL));
    int wait_time = rand() % (TU +1);

    fprintf(file, "%d: Z %d: started\n", counter++, idU);
    usleep(wait_time * 1000);
    
    
    //zjistí, jestli je pošta otevřena
    if(is_postal_open == true) {
        fprintf(file, "%d: U %d: entering office for a service %d\n", counter++, idU, queue_number);
        fprintf(file, "%d: U %d: called by office worker\n", counter++, idU);
        fprintf(file, "%d: U %d: going home\n", counter++, idU); 
    } else {
        fprintf(file, "%d: U %d: going home\n", counter++, idU); 
    }
}

//funkce pro výpočet náhodného čísla
int random_number(int lower, int upper, int count) {
   int num;
   for(int i = 0; i < count; i++)
   {
      num = (rand() % (upper - lower + 1)) + lower;
   }
   return num;
}

void clear(FILE *file) {
    fclose(file);
}

void create_processes(int processCount, int processType) {
    for(int i = 0; i < processCount; i++) {
        pid_t pid = fork();
        if(pid == -1) {
            fprintf(stderr, "Chyba: nepodařilo se vytvořit proces\n");
            exit(1);
        } else if (pid == 0) {
            if(processType == 0) {
                fprintf(stdout, "Proces zákazníka %d vytvořen\n", getpid());
            } else {
                fprintf(stdout, "Proces úředníka %d vytvořen\n", getpid());
            }
            exit(0);
        }
    }
}