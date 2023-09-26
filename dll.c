// Copyright Razvan-Constantin Rus 312CAa 2022-2023

#include "dll.h"

dl_list_t dll_create(long data_size)
{
	dl_list_t list = (dl_list_t)malloc(sizeof(dl_list));
	if (!list)
		return NULL;
	list->size = 0;
	list->head = NULL;
	list->data_size = data_size;

	return list;
}

long dll_add_nth_node(dl_list_t list, long n, void *data)
{
	if (list && n >= 0) {
		long i;
		dll_node_t new = (dll_node_t)malloc(sizeof(dll_node)), aux;
		if (verf_din_alloc(new))
			return -1;
		new->data = malloc(list->data_size);
		if (verf_din_alloc(new->data)) {
			free(new);
			return -1;
		}
		memcpy(new->data, data, list->data_size);

		if (n > list->size)
			n = list->size;

		if (list->size == 0) {
			list->head = new;
			new->next = new;
			new->prev = new;
		} else {
			if (n == 0) {
				new->next = list->head;
				new->prev = list->head->prev;
				list->head->prev->next = new;
				list->head->prev = new;
				list->head = new;
			} else {
				aux = list->head;
				for (i = 1; i < n; i++)
					aux = aux->next;
				new->next = aux->next;
				new->prev = aux;
				aux->next->prev = new;
				aux->next = new;
			}
		}
		list->size += 1;
		return 1;
	}
	return -1;
}

dll_node_t dll_remove_nth_node(dl_list_t list, long n)
{
	if (list && n >= 0 && list->size > 0) {
		dll_node_t aux;
		long i;
		aux = list->head;
		if (n >= list->size)
			n = list->size - 1;
		if (list->size == 1) {
			list->head = NULL;
		} else {
			for (i = 0; i < n; i++)
				aux = aux->next;

			aux->next->prev = aux->prev;
			aux->prev->next = aux->next;
			if (n == 0)
				list->head = aux->next;
		}

		list->size -= 1;
		return aux;
	}
	return NULL;
}

dll_node_t dll_get_nth_node(dl_list_t list, long n)
{
	if (list && n >= 0 && list->head) {
		long i;
		dll_node_t aux;

		if (n > list->size)
			n = n % list->size;

		aux = list->head;
		for (i = 0; i < n; i++)
			aux = aux->next;
		return aux;
	}
	return NULL;
}

void dll_free(dl_list_t *list, void (*free_data)(void *))
{
	if (list && *list) {
		dll_node_t aux;

		while ((*list)->size > 0) {
			aux = dll_remove_nth_node(*list, 0);
			free_data(aux->data);
			aux->data = NULL;
			my_free(aux);
			aux = NULL;
		}

		my_free(*list);
		*list = NULL;
	}
}

//lipeste lista 2 la lista 1;
long dll_merge(dl_list_t list_1, dl_list_t list_2, void (*free_func)(void *))
{
	dll_node_t aux;
	while (list_2->size) {
		aux = dll_remove_nth_node(list_2, 0);
		if (dll_add_nth_node(list_1, list_1->size, aux->data) == -1) {
			printf("Block merging failed\n");
			return -1;
		}
		my_free(aux->data);
		my_free(aux);
	}
	return 1;
}

//imparte lista 1 in doua dupa parametrul node,
//aceste ramane pe dinafara si returneaza
//lista nou formata;
dl_list_t dll_split(dl_list_t list_1, dll_node_t node, long poz)
{
	dl_list_t list_2 = dll_create(list_1->data_size);
	if (!list_2)
		return NULL;
	list_2->head = node->next;
	list_2->head->prev = list_1->head->prev;
	list_1->head->prev->next = list_2->head;

	list_1->head->prev = node->prev;
	node->prev->next = list_1->head;

	list_2->size = list_1->size - poz - 1;
	list_1->size = poz;
	return list_2;
}
