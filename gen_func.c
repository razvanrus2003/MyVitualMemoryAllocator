// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#include "gen_func.h"

//functii generice cu utilizari multiple;
//majoritatea verifica nenulitatea
//parametriilor
int my_strcmp(char *str1, char *str2)
{
	if (str1 && str2)
		return strcmp(str1, str2);
	return -1;
}

void my_free(void *data)
{
	if (data)
		free(data);
	data = NULL;
}

int verf_din_alloc(void *data)
{
	if (!data) {
		printf("Dinamic allocation failed\n");
		return 1;
	}
	return 0;
}

long my_min(long a, long b)
{
	if (a < b)
		return a;
	return b;
}
