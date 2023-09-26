// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "gen_func.h"

dl_list_t dll_create(long data_size);

long dll_add_nth_node(dl_list_t list, long n, void *data);
dll_node_t dll_remove_nth_node(dl_list_t list, long n);
dll_node_t dll_get_nth_node(dl_list_t list, long n);

void dll_free(dl_list_t *list, void (*free_val)(void *));

long dll_merge(dl_list_t list_1, dl_list_t list_2, void (*free_func)(void *));
dl_list_t dll_split(dl_list_t list_1, dll_node_t node, long poz);
