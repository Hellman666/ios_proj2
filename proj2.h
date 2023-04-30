#ifndef INPUT_CHECK_H
#define INPUT_CHECK_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/mman.h>


int input_check(int argc, char *argv[]);
int random_number(int lower, int upper);
void customer(int idZ, int TZ);
void officer(int idU, int TU);
void clear();
void create_process(int processCount, int processType);

#endif