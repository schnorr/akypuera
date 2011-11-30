/*
    This file is part of Akypuera

    Akypuera is free software: you can redistribute it and/or modify
    it under the terms of the GNU Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Akypuera is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Public License for more details.

    You should have received a copy of the GNU Public License
    along with Akypuera. If not, see <http://www.gnu.org/licenses/>.
*/
#include "aky_private.h"

typedef struct elem {
  char *data;
  struct elem *head;
  struct elem *tail;
} elem_t;

typedef struct description {
  elem_t *first;
  elem_t *last;
  int n;
} desc_t;

static void enqueue(desc_t * desc, elem_t * new)
{
  if (desc->first == NULL) {
    desc->n = 1;
    desc->first = new;
    desc->last = new;
    new->head = NULL;
    new->tail = NULL;
  } else {
    desc->n++;
    new->head = NULL;
    new->tail = desc->first;

    desc->first->head = new;
    desc->first = new;
  }
}

static elem_t *dequeue(desc_t * desc)
{
  if (desc->n == 0) {
    /* queue is empty, but someone needs a key */
    fprintf (stderr,
             "[aky_converter] at %s, the queue is empty.\n",
             __FUNCTION__);
    return NULL;
  } else {
    elem_t *ret = desc->last;
    if (ret->head) {
      ret->head->tail = NULL;
    }
    desc->last = ret->head;
    desc->n--;
    if (desc->n == 0) {
      desc->first = NULL;
    }

    ret->head = NULL;
    ret->tail = NULL;

    return ret;
  }
}

static elem_t *new_element(int src, int dst, char *key, int n)
{
  //generate the key
  static unsigned long long counter = 0;
  snprintf(key, n, "%d%d%lld", src, dst, counter++);

  elem_t *new = (elem_t *) malloc(sizeof(elem_t));
  new->data = strdup(key);
  new->head = NULL;
  new->tail = NULL;
  return new;
}

static void free_element(elem_t * elem)
{
  free(elem->data);
  free(elem);
}

char *aky_put_key(const char *type, int src, int dst, char *key, int n)
{
  char aux[100];
  snprintf(aux, 100, "%s#%d#%d", type, src, dst);
  ENTRY e, *ep;
  e.key = aux;
  e.data = NULL;

  ep = hsearch(e, FIND);
  if (ep == NULL) {
    e.data = malloc(sizeof(desc_t));
    ((desc_t *) e.data)->first = NULL;
    ((desc_t *) e.data)->last = NULL;
    ((desc_t *) e.data)->n = 0;
    ep = hsearch(e, ENTER);
  }
  elem_t *new = new_element(src, dst, key, n);
  enqueue(ep->data, new);
  return key;
}

char *aky_get_key(const char *type, int src, int dst, char *key, int n)
{
  char aux[100];
  snprintf(aux, 100, "%s#%d#%d", type, src, dst);
  ENTRY e, *ep;
  e.key = aux;
  e.data = NULL;
  ep = hsearch(e, FIND);
  if (ep == NULL) {
    return NULL;
  }
  elem_t *elem = dequeue(ep->data);
  if (elem == NULL) {
    fprintf (stderr,
             "[aky_converter] at %s, there is no key available\n"
             "[aky_converter] when type = %s, src = %d, dst = %d.\n",
             __FUNCTION__, type, src, dst);
    return NULL;
  }
  snprintf(key, n, "%s", elem->data);
  free_element(elem);
  return key;
}
