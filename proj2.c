#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#include "proj2.h"

int NZ, NU, TZ, TU, F;  /// vytovření proměnných pro vstupy
int counter = 1;        /// vytvoření counteru pro výstup   
bool is_postal_open = false;

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

    int wait_time = F/2 + rand() % (F/2 +1);
    usleep(wait_time * 1000);
    printf("%d \n", wait_time * 1000);
    is_postal_open = true;

        for(int i = 0; i <= NZ; i++) {
      customer(file, i, TZ);
    }

    fprintf(file, "%d: closing\n", counter++);
    printf("Procesy skončily\n");
    is_postal_open = false;
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

//funkce pro výpočet náhodného čísla
int random_number(int lower, int upper, int count) {
   int num;
   for(int i = 0; i < count; i++)
   {
      num = (rand() % (upper - lower + 1)) + lower;
   }
   return num;
}