// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#include "vma.h"

//functiile cerute
arena_t alloc_arena(const long size)
{
	arena_t a = (arena_t)malloc(sizeof(arena));
	verf_din_alloc(a);

	if (a) {
		a->arena_size = size;
		a->alloc_list = dll_create(sizeof(block));
		if (!a->alloc_list) {
			free(a);
			a = NULL;
		}
	}
	a->free_mem = size;
	a->miniblocks = 0;
	return a;
}

void dealloc_arena(arena_t arena)
{
	dll_free(&arena->alloc_list, dealloc_blocks);
	my_free(arena);
}

void alloc_block(arena_t a, const long addr, const long size)
{
	miniblock_t mnb = (miniblock_t)malloc(sizeof(miniblock));
	dll_node_t nx, prv;
	dl_list_t list1, list2;
	if (verf_din_alloc(mnb))
		return;
	init_miniblock(mnb, addr, size);
	block_t bl = (block_t)malloc(sizeof(block)), nx_bl, prv_bl;
	if (verf_din_alloc(bl)) {
		free(mnb);
		return;
	}
	bl->start_address = addr;
	bl->size = size;
	bl->miniblock_list = dll_create(sizeof(miniblock));
	if (verf_din_alloc(bl->miniblock_list)) {
		free(mnb);
		free(bl);
		return;
	}
	if (dll_add_nth_node(bl->miniblock_list, 0, mnb) == -1)
		printf("Miniblock allocation failed\n");
	free(mnb);
	if (a->alloc_list->size == 0 && addr + size <= a->arena_size) {
		if (dll_add_nth_node(a->alloc_list, 0, bl) == -1) {
			printf("Block allocation failed\n");
		} else {
			a->miniblocks++;
			a->free_mem -= size;
		}
		free(bl);
		return;
	}
	long poz = valid_address(a, addr, size);
	if (poz == -1) {
		dealloc_blocks(bl);
		return;
	}
	if (poz != a->alloc_list->size) {
		nx = dll_get_nth_node(a->alloc_list, poz);
		nx_bl = (block_t)nx->data;
		if (addr + size == nx_bl->start_address) {
			list1 = bl->miniblock_list;
			list2 = nx_bl->miniblock_list;
			if (dll_merge(list1, list2, dealloc_miniblocks) == -1)
				return;
			bl->size += nx_bl->size;
			nx = dll_remove_nth_node(a->alloc_list, poz);
			my_free(nx_bl->miniblock_list);
			my_free(nx_bl);
			my_free(nx);
		}
	}
	if (poz != 0) {
		prv = dll_get_nth_node(a->alloc_list, poz - 1);
		prv_bl = (block_t)prv->data;
		if (addr == prv_bl->start_address + prv_bl->size) {
			list1 = prv_bl->miniblock_list;
			list2 = bl->miniblock_list;
			if (dll_merge(list1, list2, dealloc_miniblocks) == -1)
				return;
			prv_bl->size += bl->size;
			my_free(bl->miniblock_list);
			my_free(bl);
			a->miniblocks++;
			a->free_mem -= size;
			return;
		}
	}
	if (dll_add_nth_node(a->alloc_list, poz, bl) == -1) {
		printf("Block allocation failed\n");
	} else {
		a->miniblocks++;
		a->free_mem -= size;
	}
	free(bl);
}

void free_block(arena_t a, const long addr)
{
	dll_node_t node = a->alloc_list->head;
	if (!node) {
		printf("Invalid address for free.\n");
		return;
	}
	block_t bl = (block_t)node->data;
	long i, n = a->alloc_list->size;

	for (i = 0; i < n; i++) {
		if (bl->start_address <= addr && bl->start_address + bl->size > addr)
			break;
		node = node->next;
		bl = (block_t)node->data;
	}

	if (bl->start_address <= addr && bl->start_address + bl->size > addr) {
		if (i < n)
			free_miniblock(a, i, addr);
	} else {
		printf("Invalid address for free.\n");
	}
}

void read(arena_t a, long addr, long size)
{
	dll_node_t node, r_beg = NULL;
	block_t bl;
	miniblock_t mnb;
	node = find_addr_block(a, addr);
	if (!node) {
		printf("Invalid address for read.\n");
		return;
	}
	bl = node->data;
	r_beg = find_addr_node(bl, addr, 4);
	if (!r_beg) {
		printf("Invalid permissions for read.\n");
		return;
	}

	long new_size = size;
	if (addr + size > bl->start_address + bl->size) {
		new_size = new_size - (addr + size - bl->start_address - bl->size);
		printf("Warning: size was bigger than the block size.");
		printf(" Reading %ld characters.\n", new_size);
	}
	mnb = (miniblock_t)r_beg->data;
	int offset = addr - mnb->start_address, min;

	char *aux;
	while (new_size > 0) {
		if (!mnb->rw_buffer) {
			printf("\n");
			return;
		}
		min = my_min(new_size, mnb->size - offset);
		aux = calloc(min + 1, sizeof(char));
		if (verf_din_alloc(aux))
			return;
		memcpy(aux, mnb->rw_buffer + offset, min);
		aux[min] = '\0';
		printf("%s", aux);
		free(aux);
		new_size -= min;
		offset = 0;
		r_beg = r_beg->next;
		mnb = (miniblock_t)r_beg->data;
	}
	printf("\n");
}

void write(arena_t a, const long addr, const long size, void *data)
{
	dll_node_t node, w_beg = NULL;
	block_t bl;
	miniblock_t mnb;
	node = find_addr_block(a, addr);
	if (!node) {
		printf("Invalid address for write.\n");
		return;
	}
	bl = node->data;
	w_beg = find_addr_node(bl, addr, 2);
	if (!w_beg) {
		printf("Invalid permissions for write.\n");
		return;
	}

	long new_size = size;
	if (addr + size > bl->start_address + bl->size) {
		new_size = new_size - (addr + size - bl->start_address - bl->size);
		printf("Warning: size was bigger than the block size.");
		printf(" Writing %ld characters.\n", new_size);
	}

	mnb = (miniblock_t)w_beg->data;
	long offset = addr - mnb->start_address, min;
	while (new_size > 0) {
		if (!mnb->rw_buffer)
			mnb->rw_buffer = calloc(mnb->size, sizeof(char));
		if (verf_din_alloc(mnb->rw_buffer))
			return;
		min = my_min(new_size, mnb->size - offset);
		memcpy(mnb->rw_buffer + offset, data, min);
		data = data + min;
		new_size -= min;
		offset = 0;
		w_beg = w_beg->next;
		mnb = (miniblock_t)w_beg->data;
	}
}

void pmap(const arena_t arena)
{
	long n = arena->alloc_list->size, j, m, i, end_addr;
	printf("Total memory: 0x%lX bytes\n", arena->arena_size);
	printf("Free memory: 0x%lX bytes\n", arena->free_mem);
	printf("Number of allocated blocks: %ld\n", n);
	printf("Number of allocated miniblocks: %ld\n", arena->miniblocks);
	char *perm = (char *)calloc(4, sizeof(char));
	if (verf_din_alloc(perm))
		return;
	dll_node_t bl_node = arena->alloc_list->head, mnb_node;
	block_t bl;
	miniblock_t mnb;
	for (i = 0; i < n; i++) {
		bl = (block_t)bl_node->data;
		mnb_node = (dll_node_t)bl->miniblock_list->head;
		mnb = (miniblock_t)mnb_node->prev->data;
		end_addr = mnb->start_address + mnb->size;
		printf("\nBlock %ld begin\n", i + 1);
		printf("Zone: 0x%lX - 0x%lX\n", bl->start_address, end_addr);

		m = bl->miniblock_list->size;

		mnb = (miniblock_t)mnb_node->data;
		for (j = 0; j < m; j++) {
			printf("Miniblock %ld:\t\t0x", j + 1);
			printf("%lX\t\t-\t\t0x", mnb->start_address);
			printf("%lX\t\t| ", mnb->start_address + mnb->size);
			printf("%s\n", perm_rev(perm, mnb->perm));
			mnb_node = mnb_node->next;
			mnb = (miniblock_t)mnb_node->data;
		}
		bl_node = bl_node->next;
		printf("Block %ld end\n", i + 1);
	}
	free(perm);
}

void mprotect(arena_t a, long addr, long *permission)
{
	dll_node_t node, p_beg = NULL;
	block_t bl;
	miniblock_t mnb;
	node = find_addr_block(a, addr);
	if (!node) {
		printf("Invalid address for mprotect.\n");
		//printf("222\n");
		return;
	}
	bl = node->data;
	p_beg = find_addr_node(bl, addr, 0);
	if (!p_beg) {
		printf("Invalid address for mprotect.\n");
		//printf("111\n");
		return;
	}

	long fin_bl = bl->start_address + bl->size;
	long perm = *permission;
	mnb = p_beg->data;
	if (addr != mnb->start_address) {
		printf("Invalid address for mprotect.\n");
		//printf("111\n");
		return;
	}
	//printf("%ld\n", mnb->start_address);
	while (1) {
		mnb->perm = perm;
		if (mnb->start_address + mnb->size >= fin_bl)
			break;
		p_beg = p_beg->next;
		mnb = (miniblock_t)p_beg->data;
	}
}
