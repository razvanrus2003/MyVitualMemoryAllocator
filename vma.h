// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include "structs.h"
#include "dll.h"
#include "gen_func.h"
#include "aux_func.h"

arena_t alloc_arena(const long size);
void dealloc_arena(arena_t arena);

void alloc_block(arena_t a, const long addr, const long size);
void free_block(arena_t a, const long addr);

void read(arena_t arena, long address, long size);
void write(arena_t arena, const long address,  const long size, void *data);
void pmap(const arena_t arena);
void mprotect(arena_t arena, long address, long *permission);
