// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int my_strcmp(char *str1, char *str2);
char *valid_command(char *command);
void my_free(void *data);
int verf_din_alloc(void *data);
long my_min(long a, long b);
