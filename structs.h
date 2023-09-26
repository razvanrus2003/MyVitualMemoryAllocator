// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct dll_node {
	struct dll_node *next;
	struct dll_node *prev;
	void *data;
} *dll_node_t, dll_node;

typedef struct dl_list {
	long size;
	long data_size;
	dll_node_t head;
} *dl_list_t, dl_list;

typedef struct miniblock {
	long start_address;
	long size;
	long perm;
	void *rw_buffer;
} *miniblock_t, miniblock;

typedef struct block {
	long start_address;
	long size;
	dl_list_t miniblock_list;
} *block_t, block;

typedef struct arena {
	long arena_size;
	dl_list_t alloc_list;
	long miniblocks;
	long free_mem;
} *arena_t, arena;
