#ifndef INPUT_CHECK_H
#define INPUT_CHECK_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <semaphore.h>

int input_check(int NU, int NZ, int TZ, int TU, int F);
int random_number(int lower, int upper, int count);
void customer(FILE *file, int idZ, int TZ);
void open_postal_office();
void close_postal_office();
void clear(FILE *file);

#endif