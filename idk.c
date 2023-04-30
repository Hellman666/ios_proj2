#include "proj2.h"

int main(int argc, char *argv[]) {
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

    is_postal_open = true;

    // vytvoření procesů zákazníků
    create_process(NZ, 0);
    
    // vytvoření procesů úředníků
    create_process(NU, 1);





    fprintf(file, "%d: closing\n", counter++);
    printf("Procesy skončily\n");

    clear(file);   
    while(wait(NULL) > 0);
    printf("Načtené argumenty:\nNZ: %d\nNU: %d\nTZ: %d\nTU: %d\nF: %d\n", NZ, NU, TZ, TU, F);

    return 0;
}

void customer(FILE *file, int idZ, int TZ) {
    fprintf(file, "%d: Z %d: started\n", counter++, idZ);
    usleep((rand() % (TZ +1)) * 1000);
    
    if(is_postal_open == false){
        fprintf(file, "%d: Z %d: going home\n", counter++, idZ);
        exit(0);
    }


    queue = true;
    fprintf(file, "%d: Z %d: entering office for a service %d\n", counter++, idZ, (rand() % 3 + 1));
    fprintf(file, "Z %d: called by office worker\n", idZ);
    
    // vyčkání náhodné doby na dokončení žádosti
    
    usleep((rand() % (TZ + 1)) * 1000);

    fprintf(file, "%d: Z %d: going home\n", counter++, idZ); 
    queue = false;

    exit(0);
}

void officer(FILE *file, int idU, int TU) {
    fprintf(file, "%d: U %d: started\n", counter++, idU);
    fprintf(file, "%d: U %d: serving a service of type %d\n", counter++, idU, (rand() % 3 + 1));
    usleep((rand() % 11) * 1000);
    fprintf(file, "%d: U %d: service finished\n", counter++, idU);

    //pokud v žádné frontě nečeká zákazní a pošta je otevřená
    if(queue == false && is_postal_open == true){
        fprintf(file, "%d: U %d: taking break\n", counter++, idU);
        usleep((rand() % (TU + 1))*1000);
        fprintf(file, "%d: U %d: break finished\n", counter++, idU);
    }
    
    if(is_postal_open == false){
        fprintf(file, "%d: U %d: going home\n", counter++, idU); 
    }
    exit(0);
}

void create_process(int processCount, int processType) {
    for(int i = 0; i < processCount; i++) {
        pid_t pid = fork();
        if(pid == -1) {
            fprintf(stderr, "Chyba: nepodařilo se vytvořit proces\n");
            exit(1);
        } else if (pid == 0) {
            if(processType == 0) {
                //customer(file, );
                customer(file, i, TZ);
                fprintf(stdout, "Proces zákazníka %d vytvořen\n", getpid());
            } else {
                officer(file, i, TU);
                fprintf(stdout, "Proces úředníka %d vytvořen\n", getpid());
            }
            exit(0);
        }
    }
}

void clear(FILE *file) {
    //sem_destroy();
    //munmap(mutex, sizeof(sem_t));
    fclose(file);
}

int input_check(int argc, char *argv[]) {  
    //TODO: prý může být počet úředníků 0. Pokud tomu tak je, tak udělat program, aby s tím počítal
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
