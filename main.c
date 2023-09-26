// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vma.h"

int main(void)
{
	char *com, *token, *r_buf;
	long pr1, pr2, extra, run = -1;
	arena_t arena;
	while (run) {
		scanf("\n");
		scanf("%m[^\n]%*c", &com);
		token = valid_command(com);
		if (!my_strcmp(token, "ALLOC_ARENA")) {
			token = strtok(NULL, "\n");
			if (token && sscanf(token, "%ld %ld", &pr1, &extra) == 1)
				arena = alloc_arena(pr1);
			else
				print_invalid_command(token, 1);
		}
		if (!my_strcmp(token, "PMAP")) {
			token = strtok(NULL, "\n");
			if (!token)
				pmap(arena);
			else
				print_invalid_command(token, 1);
		}
		if (!my_strcmp(token, "DEALLOC_ARENA")) {
			token = strtok(NULL, "\n");
			if (!token) {
				dealloc_arena(arena);
				run = 0;
			} else {
				print_invalid_command(token, 1);
			}
		}
		if (!my_strcmp(token, "ALLOC_BLOCK")) {
			token = strtok(NULL, "\n");
			if (token && sscanf(token, "%ld %ld %ld", &pr1, &pr2, &extra) == 2)
				alloc_block(arena, pr1, pr2);
			else
				print_invalid_command(token, 1);
		}
		if (!my_strcmp(token, "FREE_BLOCK")) {
			token = strtok(NULL, "\n");
			if (token && sscanf(token, "%ld %ld", &pr1, &extra) == 1)
				free_block(arena, pr1);
			else
				print_invalid_command(token, 1);
		}
		if (!my_strcmp(token, "WRITE")) {
			token = strtok(NULL, "\n");
			if (token && sscanf(token, "%ld %ld", &pr1, &pr2) == 2) {
				r_buf = extract_w_data(token);
				if (r_buf)
					write(arena, pr1, pr2, r_buf);
				my_free(r_buf);
			} else {
				print_invalid_command(token, 1);
			}
		}
		if (!my_strcmp(token, "READ")) {
			token = strtok(NULL, "\n");
			if (token && sscanf(token, "%ld %ld %ld", &pr1, &pr2, &extra) == 2)
				read(arena, pr1, pr2);
			else
				print_invalid_command(token, 1);
		}
		if (!my_strcmp(token, "MPROTECT")) {
			token = strtok(NULL, "\n");
			if (token && sscanf(token, "%ld %m[^\n]%*c", &pr1, &r_buf) == 2) {
				long perm = perm_con(r_buf);
				if (perm == -1)
					print_invalid_command(r_buf, 2);
				else
					mprotect(arena, pr1, &perm);
				my_free(r_buf);
			} else {
				print_invalid_command(token, 1);
			}
		}
		free(com);
	}
	return 0;
}
