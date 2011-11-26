/*
    Copyright (c) 1998--2006 Benhur Stein
    
    This file is part of Pajé.

    Pajé is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    Pajé is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
    for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Pajé; if not, write to the Free Software Foundation, Inc.,
	51 Franklin Street, Fifth Floor, Boston, MA 02111 USA.
        
    Author: Geovani Ricardo Wiedenhoft
    Email: grw@inf.ufsm.br
*/
#ifndef _RASTRO_LIST_H_
#define _RASTRO_LIST_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/**Dado da lista**/
typedef void *list_data_t;

typedef struct _list_element {
  list_data_t data;
  struct _list_element *prev;
  struct _list_element *next;
} list_element_t;

typedef list_element_t *position_t;

typedef void (*list_func_copy_t) (list_element_t *, list_data_t);
typedef bool(*list_func_compare_t) (list_data_t, list_data_t);
typedef void (*list_func_destroy_t) (position_t);


typedef struct {
  list_element_t *sent;
  list_func_copy_t list_copy;
  list_func_compare_t list_compare;
  list_func_destroy_t list_destroy;
} list_t;


void list_null(void *p);

void list_initialize(list_t * list, list_func_copy_t copy,
                     list_func_compare_t compare,
                     list_func_destroy_t destroy);

void list_finalize(list_t * list);

void list_destroy(position_t pos);
void list_destroy_int(position_t pos);
void list_destroy_string(position_t pos);


position_t list_locate(list_t * list, list_data_t data);

bool list_find(list_t * list, list_data_t data);

bool list_cmp(list_data_t, list_data_t);
bool list_cmp_int(list_data_t, list_data_t);
bool list_cmp_string(list_data_t, list_data_t);

void list_copy(list_element_t * element, list_data_t data);
void list_copy_int(list_element_t * element, list_data_t data);
void list_copy_string(list_element_t * element, list_data_t data);

/* retorna a posicao do primeiro elemento da lista ou NULL se lista vazia */
position_t list_inicio(list_t * list);

/* retorna a posicao do ultimo elemento da lista ou NULL se lista vazia */
position_t list_final(list_t * list);

/* insere dado apos pos (se pos for NULL, insere no inicio da lista) */
void list_insert_after(list_t * list, position_t pos, list_data_t data);

/* insere dado antes de pos (se pos for NULL, insere no final da lista) */
void list_insert_before(list_t * list, position_t pos, list_data_t data);

void list_remove(list_t * list, list_data_t data);

/* remove o elemento na position pos */
/*(se pos igual a NULL remove o primeiro elemento da lista)*/
void list_rem_position(list_t * list, position_t pos);

/* retorna o dado contido na posicao pos */
list_data_t list_dado_position(list_t * list, position_t pos);

/* retorna a posicao do elemento que segue o elemento na posicao pos
   (ou NULL, se pos representa o ultimo elemento ou se pos for NULL) */
position_t list_avanca_position(list_t * list, position_t pos);

/* retorna a posicao do elemento que antecede o elemento na posicao pos
   (ou NULL, se pos representa o primeiro elemento ou se pos for NULL) */
position_t list_recua_position(list_t * list, position_t pos);

/* retorna o numero de elementos da lista */
int list_quantity(list_t * list);




#endif                          //_RASTRO_LIST_H_
