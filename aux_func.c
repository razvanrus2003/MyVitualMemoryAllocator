// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#include "aux_func.h"

//functii auxiliare apelate in program;
long valid_address(arena_t a, const long addr, const long size)
{
	if (addr + size > a->arena_size) {
		if (addr >= a->arena_size)
			printf("The allocated address is outside the size of arena\n");
		else if (addr + size > a->arena_size)
			printf("The end address is past the size of the arena\n");
		return -1;
	}
	long i = 0;
	dll_node_t curr = a->alloc_list->head;
	block_t bl = (block_t)curr->data;

	while (i < a->alloc_list->size) {
		if (bl->start_address >= addr)
			break;
		i++;
		curr = curr->next;
		bl = (block_t)curr->data;
	}

	bl = (block_t)a->alloc_list->head->prev->data;
	if (bl->start_address + bl->size <= addr)
		if (i == a->alloc_list->size)
			return i;
	bl = (block_t)curr->data;
	if (addr + size <= bl->start_address) {
		if (i > 0) {
			bl = (block_t)curr->prev->data;
			if (bl->start_address + bl->size <= addr)
				return i;
			printf("This zone was already allocated.\n");
			return -1;
		}
		return i;
	}
	printf("This zone was already allocated.\n");
	return -1;
}

void dealloc_miniblocks(void *data)
{
	miniblock_t mnb = (miniblock_t)data;
	my_free(mnb->rw_buffer);
	my_free(mnb);
}

void dealloc_blocks(void *data)
{
	block_t bl = (block_t)data;
	dll_free(&bl->miniblock_list, dealloc_miniblocks);
	my_free(bl);
}

void free_miniblock(arena_t a, long poz, const long addr)
{
	dll_node_t node = dll_get_nth_node(a->alloc_list, poz);
	block_t bl = (block_t)node->data;
	long i;
	node = bl->miniblock_list->head;
	miniblock_t mnb = (miniblock_t)node->data;

	for (i = 0; i < bl->miniblock_list->size; i++) {
		if (mnb->start_address == addr)
			break;
		node = node->next;
		mnb = node->data;
	}
	if (mnb->start_address != addr) {
		printf("Invalid address for free.\n");
		return;
	}

	a->free_mem += mnb->size;
	a->miniblocks--;

	if (i == 0 || i == bl->miniblock_list->size - 1) {
		if (bl->miniblock_list->size == 1) {
			node = dll_remove_nth_node(a->alloc_list, poz);
			dealloc_blocks(bl);
			my_free(node);
			return;
		}

		node = dll_remove_nth_node(bl->miniblock_list, i);
		bl->size -= mnb->size;
		dealloc_miniblocks(mnb);
		mnb = (miniblock_t)bl->miniblock_list->head->data;
		bl->start_address = mnb->start_address;
		my_free(node);
		return;
	}
	dl_list_t tmp = dll_split(bl->miniblock_list, node, i);
	dealloc_miniblocks(mnb);
	my_free(node);
	if (!tmp) {
		printf("Block splitting failed\n");
		return;
	}

	mnb = (miniblock_t)bl->miniblock_list->head->prev->data;
	bl->size = mnb->start_address + mnb->size - 1;
	mnb = (miniblock_t)bl->miniblock_list->head->data;
	bl->size = bl->size - mnb->start_address + 1;

	mnb = (miniblock_t)tmp->head->data;
	bl = (block_t)malloc(sizeof(block));
	if (!bl) {
		printf("Block allocation failed\n");
		return;
	}
	bl->start_address = mnb->start_address;
	bl->miniblock_list = tmp;
	mnb = (miniblock_t)tmp->head->prev->data;
	bl->size = mnb->start_address + mnb->size - 1;
	mnb = (miniblock_t)tmp->head->data;
	bl->size = bl->size - mnb->start_address + 1;

	if (dll_add_nth_node(a->alloc_list, poz + 1, bl) == -1) {
		printf("Block allocation failed\n");
		return;
	}
	free(bl);
}

void print_invalid_command(char *com, long n)
{
	if (com)
		n += count_words(com);
	int i;
	for (i = 0; i < n; i++)
		printf("Invalid command. Please try again.\n");
}

long count_words(char *com)
{
	long cont = 0;
	size_t i;
	for (i = 0; i < strlen(com); i++) {
		if (com[i] == ' ' || com[i] == '\n')
			cont++;
	}
	if (cont)
		cont++;
	return cont;
}

char *valid_command(char *command)
{
	int ok = 1;
	char *token = strtok(command, "\n ");
	if (!strcmp(command, "DEALLOC_ARENA"))
		ok = 0;
	if (!strcmp(command, "ALLOC_ARENA"))
		ok = 0;
	if (!strcmp(command, "ALLOC_BLOCK"))
		ok = 0;
	if (!strcmp(command, "FREE_BLOCK"))
		ok = 0;
	if (!strcmp(command, "READ"))
		ok = 0;
	if (!strcmp(command, "WRITE"))
		ok = 0;
	if (!strcmp(command, "PMAP"))
		ok = 0;
	if (!strcmp(command, "MPROTECT"))
		ok = 0;

	if (ok) {
		token = strtok(NULL, "\n");
		print_invalid_command(token, 1);
	}
	return token;
}

dll_node_t find_addr_block(arena_t a, long addr)
{
	dll_node_t node;
	block_t bl;
	if (a->alloc_list->size) {
		node = a->alloc_list->head;
		bl = (block_t)node->data;
		int i;
		for (i = 0; i < a->alloc_list->size; i++) {
			if (bl->start_address <= addr)
				if (bl->start_address + bl->size > addr)
					break;
			node = node->next;
			bl = (block_t)node->data;
		}

		if (i == a->alloc_list->size)
			return NULL;
		return node;
	}
	return NULL;
}

dll_node_t find_addr_node(block_t bl, long addr, long perm)
{
	dll_node_t node = bl->miniblock_list->head, beg = NULL;
	miniblock_t mnb = (miniblock_t)node->data;
	int ok = 1;
	long i;

	for (i = 0; i < bl->miniblock_list->size; i++) {
		if (mnb->start_address <= addr && !beg)
			if (addr < mnb->start_address + mnb->size)
				beg = node;

		if (beg && (mnb->perm & perm) == 0 && perm) {
			ok = 0;
			break;
		}
		node = node->next;
		mnb = (miniblock_t)node->data;
	}
	if (!ok)
		return NULL;
	return beg;
}

long perm_con(char *s)
{
	long perm = 0;
	char *token = strtok(s, " |\n");

	while (token) {
		if (!strcmp(token, "PROT_NONE"))
			perm = 0;
		if (!strcmp(token, "PROT_READ"))
			perm += 4;
		if (!strcmp(token, "PROT_WRITE"))
			perm += 2;
		if (!strcmp(token, "PROT_EXEC"))
			perm += 1;
		token = strtok(NULL, " |\n");
	}
	return perm;
}

char *perm_rev(char *s, long perm)
{
	strcpy(s, "---");
	if (!perm)
		return s;
	if ((perm & 4) != 0)
		s[0] = 'R';
	if ((perm & 2) != 0)
		s[1] = 'W';
	if ((perm & 1) != 0)
		s[2] = 'X';
	return s;
}

char *extract_w_data(char *s)
{
	size_t size, aux = 0;
	char *data, *tmp;

	if (sscanf(s, "%*d %ld %m[^\n]", &size, &data) < 2)
		data = NULL;

	if (data)
		aux = strlen(data);
	data = (char *)realloc(data, (size + 1) * sizeof(char));
	if (verf_din_alloc(data))
		return NULL;
	data[my_min(size, aux)] = '\0';

	if (aux < size) {
		data[aux] = '\n';
		data[aux + 1] = '\0';
	}

	while (strlen(data) < size) {
		scanf("%m[^\n]", &tmp);
		scanf("%*c");
		if (tmp)
			strcat(data, tmp);

		if (strlen(data) < size) {
			aux = strlen(data);
			data[aux] = '\n';
			data[aux + 1] = '\0';
		}
		my_free(tmp);
	}

	if (strlen(data) > size)
		print_invalid_command(data + size, 0);

	return data;
}

void init_miniblock(miniblock_t mnb, long addr, long size)
{
	mnb->rw_buffer = NULL;
	mnb->perm = 6;
	mnb->size = size;
	mnb->start_address = addr;
}
