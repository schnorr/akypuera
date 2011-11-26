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
#include "rastro_list.h"

void list_null(void *p)
{
  if (p == NULL) {
    printf("LIST_ERROR: Pointer is NULL\n");
    exit(1);
  }
}

void list_initialize(list_t * list, list_func_copy_t copy,
                     list_func_compare_t compare,
                     list_func_destroy_t destroy)
{
  list_null(list);

  list->sent = (list_element_t *) malloc(sizeof(*list->sent));
  if (list->sent == NULL) {
    printf("LIST_ERROR: Cannot alloc memory\n");
    exit(1);
  }

  list->sent->next = list->sent->prev = list->sent;
  list->list_copy = copy;
  list->list_compare = compare;
  list->list_destroy = destroy;

}


void list_finalize(list_t * list)
{
  position_t p;

  list_null(list);

  p = list->sent->next;
  while (p != list->sent) {
    position_t vitima;
    vitima = p;
    p = p->next;
    list->list_destroy(vitima);
    free(vitima);
  }
  free(list->sent);
  list->sent = NULL;
}

void list_destroy(position_t pos)
{
  list_null(pos);

  pos->data = NULL;
  pos->next = NULL;
  pos->prev = NULL;
}

void list_destroy_int(position_t pos)
{
  list_null(pos);

  free(pos->data);
  pos->data = NULL;
  pos->next = NULL;
  pos->prev = NULL;
}

void list_destroy_string(position_t pos)
{
  list_null(pos);

  free(pos->data);
  pos->data = NULL;
  pos->next = NULL;
  pos->prev = NULL;
}

position_t list_locate(list_t * list, list_data_t data)
{
  list_null(list);
  list_null(data);

  position_t element;
  for (element = list->sent->next; element != list->sent;
       element = element->next) {
    if (list->list_compare(element->data, data)) {
      return element;
    }

  }
  return NULL;
}

bool list_find(list_t * list, list_data_t data)
{
  list_null(list);
  list_null(data);

  position_t element;
  element = list_locate(list, data);
  if (element != NULL) {
    return true;
  }
  return false;

}

bool list_cmp(list_data_t data, list_data_t data2)
{
  list_null(data);
  list_null(data2);

  if (data == data2) {
    return true;
  }
  return false;

}

bool list_cmp_int(list_data_t data, list_data_t data2)
{
  list_null(data);
  list_null(data2);

  if (!memcmp(data, data2, sizeof(int))) {
    return true;
  }
  return false;

}

bool list_cmp_string(list_data_t data, list_data_t data2)
{
  list_null(data);
  list_null(data2);

  if (!strcmp(data, data2)) {
    return true;
  }
  return false;

}

void list_copy(list_element_t * element, list_data_t data)
{
  list_null(element);
  list_null(data);

  element->data = (void *) data;
}

void list_copy_int(list_element_t * element, list_data_t data)
{
  list_null(element);
  list_null(data);

  element->data = (void *) malloc(sizeof(int));
  element->data = memcpy(element->data, data, sizeof(int));
}

void list_copy_string(list_element_t * element, list_data_t data)
{
  list_null(element);
  list_null(data);

  element->data =
      (void *) malloc((sizeof(char) * strlen((char *) data)) + 1);
  element->data = strcpy(element->data, data);
}

/* retorna a position do primeiro elemento da list ou NULL se list vazia */
position_t list_inicio(list_t * list)
{
  list_null(list);

  if (list->sent->next != list->sent) {
    return list->sent->next;
  } else {
    return NULL;
  }
}

/* retorna a position do ultimo elemento da list ou NULL se list vazia */
position_t list_final(list_t * list)
{
  list_null(list);

  if (list->sent->prev != list->sent) {
    return list->sent->prev;
  } else {
    return NULL;
  }
}


/* insere dado apos pos (se pos for NULL, insere no inicio da list) */
void list_insert_after(list_t * list, position_t pos, list_data_t data)
{
  list_element_t *new;

  list_null(list);
  list_null(data);

  new = (list_element_t *) malloc(sizeof(*new));
  if (new == NULL) {
    printf("LIST_ERROR: Cannot alloc memory\n");
    exit(1);
  }
  list->list_copy(new, data);
  //new->data=data;
  if (pos == NULL) {
    new->prev = list->sent;
    new->next = list->sent->next;
    list->sent->next->prev = new;
    list->sent->next = new;
  } else {
    new->prev = pos;
    new->next = pos->next;
    pos->next->prev = new;
    pos->next = new;
  }
}

/* insere dado antes de pos (se pos for NULL, insere no final da list) */
void list_insert_before(list_t * list, position_t pos, list_data_t data)
{
  list_element_t *new;

  list_null(list);
  list_null(data);

  new = (list_element_t *) malloc(sizeof(*new));
  if (new == NULL) {
    printf("LIST_ERROR: Cannot alloc memory\n");
    exit(1);
  }
  list->list_copy(new, data);
  //new->data=data;
  if (pos == NULL) {
    new->next = list->sent;
    new->prev = list->sent->prev;
    list->sent->prev->next = new;
    list->sent->prev = new;
  } else {
    new->prev = pos->prev;
    new->next = pos;
    pos->prev->next = new;
    pos->prev = new;
  }
}

void list_remove(list_t * list, list_data_t data)
{
  position_t pos;

  list_null(list);
  list_null(data);

  pos = list_locate(list, data);
  if (pos == NULL) {
    printf("LIST_ERROR: Cannot remove List element\n");
    exit(1);
  }
  list_rem_position(list, pos);



}

/* remove o elemento na position pos */
/*(se pos igual a NULL remove o primeiro elemento da lista)*/
void list_rem_position(list_t * list, position_t pos)
{
  list_null(list);

  if (list_quantity(list) == 0) {
    return;
  }

  if (pos == NULL) {
    position_t tmp;
    tmp = list->sent->next;
    tmp->next->prev = list->sent;
    list->sent->next = tmp->next;
    list->list_destroy(tmp);
    return;
  } else {
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;
    list->list_destroy(pos);
  }
}


/* retorna o dado contido na position pos */
list_data_t list_data_position(list_t * list, position_t pos)
{
  list_null(list);
  list_null(pos);

  return pos->data;
}

/* retorna a position do elemento que segue o elemento na position pos
   (ou NULL, se pos representa o ultimo elemento ou se pos for NULL) */
position_t list_avanca_position(list_t * list, position_t pos)
{
  list_null(list);

  if (pos == NULL || pos->next == list->sent) {
    return NULL;
  }
  return pos->next;
}

/* retorna a position do elemento que antecede o elemento na position pos
   (ou NULL, se pos representa o primeiro elemento ou se pos for NULL) */
position_t list_recua_position(list_t * list, position_t pos)
{
  list_null(list);

  if (pos == NULL || pos->prev == list->sent) {
    return NULL;
  }
  return pos->prev;
}

/* retorna o numero de elementos da list */
int list_quantity(list_t * list)
{
  int i = 0;
  position_t element;

  list_null(list);
  list_null(list->sent);

  for (element = list->sent->next; element != list->sent;
       element = element->next) {
    i++;
  }
  return i;
}
