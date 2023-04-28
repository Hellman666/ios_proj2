/**
 * @file proj2.c
 * @author Dominik Borek (xborek12@vutbr.cz)
 * @brief Program pro synchronizaci pošty
 * @version 0.1
 * @date 2023-04-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "proj2.h"

int NZ, NU, TZ, TU, F;  /// vytovření proměnných pro vstupy
int counter = 1;        /// vytvoření counteru pro výstup   
sem_t* semaphore = NULL;
bool postal_office_open = false;


/**
 * @brief Funkce main programu
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
   if(argc != 6) {
    printf("Chybný počet načtených argumentů\n");
    return 1;
   }

   //otevření souboru proj2.out
   FILE *file;
   file = fopen("proj2.out", "w");

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
   
   
   for(int i = 0; i < NZ; i++) {
      pid_t pid = fork();
      if(pid == -1) {
         fprintf(stderr, "Chyba: nepodařilo se vytvořit proces zákazníka\n");
         return 1;
      } else if (pid == 0) {
         fprintf(stdout, "Proces zákazníka %d vytvořen\n", getpid());
         return 0;
      }
   }
   for(int i = 0; i < NU; i++) {
      pid_t pid = fork();
      if (pid == -1)
      {
         fprintf(stderr, "Chyba: nepodařilo se vytvořit proces úředníka\n");
         return 1;
      } else if (pid == 0) {
         fprintf(stdout, "Proces úředníka %d vytvořen\n", getpid());
         return 0;
      }
   }
   open_postal_office();

   int wait_time = F/2 + rand() % (F/2 +1);
   usleep(wait_time * 1000);
   //fprintf(file, "sleep\n");
   //printf("sleep\n");

   //TODO: pokud bude čas od otevření pošty == F, zavolá se funkce close_postal_office()

   for(int i = 0; i <= NZ; i++) {
      customer(file, i, TZ);
   }

   wait (NULL);
   fprintf(file, "%d: closing\n", counter++);
   printf("Procesy skončily\n");
   

   close_postal_office();
   fclose(file);   

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

   //inicializace semaforu
   if (semaphore == NULL) {
      semaphore = sem_open("postal_sem", O_CREAT, 0666, 1);
   }

   //vytvoření náhodného čísla od 1 do 3
   int x = random_number(1, 3, 1);   

   srand(time(NULL));
   int wait_time = rand() % (TZ +1);

   fprintf(file, "%d: Z %d: started\n", counter++, idZ);
   usleep(wait_time * 1000);
   //printf("%d \n", wait_time*1000);
   
   //získání semaforu
   sem_wait(semaphore);

   //zjistí, jestli je pošta otevřena
   if(postal_office_open) {
      fprintf(file, "%d: Z %d: entering office for a service %d\n", counter++, idZ, x);
      fprintf(file, "%d: Z %d: called by office worker\n", counter++, idZ);
      fprintf(file, "%d: Z %d:  going home\n", counter++, idZ); 
   } else {
      fprintf(file, "%d: Z %d:  going home\n", counter++, idZ); 
   }

   //uvolnění semaforu
   sem_post(semaphore);
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

// function to open the postal office
void open_postal_office() {
    postal_office_open = true;
}

// function to close the postal office
void close_postal_office() {
    postal_office_open = false;
}

void clerk(FILE *file, int idU, int TU, queue_t **queues) {

   // inicializace náhodného generátoru
   srand(time(NULL));

   fprintf(file, "%d: U %d: started\n", counter++, idU);
   
   while (1) {
      int i;
      for (i = 0; i < 3; i++) {
         //TODO: vybere náhodnou neprázdnou frontu
         if (!queue_is_empty(queues[i])) {
            int service_type = i+1;
            fprintf(file, "%d: U %d: serving a service of type %d\n", counter++, idU, service_type);

            // obsluha zákazníka
            usleep(random_number(0, 10, 1) * 1000);
            fprintf(file, "%d: U %d: service finished\n", counter++, idU);

            // odebrání zákazníka z fronty
            queue_pop(queues[i]);

            // pokud byla fronta prázdná, zkontroluje se stav pošty
            if (queue_is_empty(queues[i])) {
               if (postal_office_open) {
                  fprintf(file, "%d: U %d: taking break\n", counter++, idU);
                  usleep(random_number(0, TU, 1) * 1000);
                  fprintf(file, "%d: U %d: break finished\n", counter++, idU);
               } else {
                  fprintf(file, "%d: U %d: going home\n", counter++, idU);
                  return;
               }
            }
            break;
         }
      }

      // pokud nebyla nalezena žádná fronta s neprázdnou frontou
      if (i == 3) {
         if (postal_office_open) {
            fprintf(file, "%d: U %d: taking break\n", counter++, idU);
            usleep(random_number(0, TU, 1) * 1000);
            fprintf(file, "%d: U %d: break finished\n", counter++, idU);
         } else {
            fprintf(file, "%d: U %d: going home\n", counter++, idU);
            return;
         }
      }
   }
}

//TODO: vytvořit funkci queue_is_empty, ve které se projdou všechny fronty a zjistí se, která je prázdná/prázdné

