// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include "structs.h"
#include "dll.h"
#include "gen_func.h"

long valid_address(arena_t a, const long addr, const long size);

void dealloc_miniblocks(void *data);
void dealloc_blocks(void *data);

void free_miniblock(arena_t a, long poz, const long addr);

void print_invalid_command(char *com, long n);
long count_words(char *com);
char *valid_command(char *command);

dll_node_t find_addr_block(arena_t a, long addr);
dll_node_t find_addr_node(block_t bl, long addr, long perm);

long perm_con(char *s);
char *perm_rev(char *s, long perm);
char *extract_w_data(char *s);

void init_miniblock(miniblock_t mnb, long addr, long size);
