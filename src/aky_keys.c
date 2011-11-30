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
#define _GNU_SOURCE
#define __USE_GNU
#include <search.h>

static struct hsearch_data hash;


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

static void print_queue (desc_t *desc)
{
  /* debug list elements */
  elem_t *it = desc->first;
  while (it != NULL){
    fprintf (stderr,
             "  desc=%p (first=%p, last=%p) it=%p (head=%p, tail=%p)\n",
             desc, desc->first, desc->last,
             it, it->head, it->tail);
    it = it->tail;
  }
}

static void enqueue(desc_t * desc, elem_t * new)
{
  if (desc->n == 0){
    /* queue is empty, add in the beginning */
    desc->n++;
    desc->first = new;
    desc->last = new;

    /* new element is not linked with others elements (because it is alone) */
    new->head = NULL;
    new->tail = NULL;
  }else{
    /* queue is NOT empty, add at the end */
    desc->n++;

    elem_t *old_last = desc->last;
    old_last->tail = new;
    new->head = old_last;
    new->tail = NULL;
    desc->last = new;
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
    /* queue is NOT empty, that's good, get the first element */
    elem_t *ret = desc->first;
    elem_t *new_first = desc->first->tail;
    if (new_first){
      new_first->head = NULL;
      desc->first = new_first;
    }else{
      desc->first = NULL;
      desc->last = NULL;
    }
    desc->n--;

    ret->head = NULL;
    ret->tail = NULL;

    return ret;
  }
}

static elem_t *new_element(int src, int dst, char *key, int n)
{
  //zeroe the key
  bzero (key, n);

  //generate the key
  static unsigned long long counter = 0;
  snprintf(key, n, "%d%d%lld", src, dst, counter++);

  elem_t *new = (elem_t *) malloc(sizeof(elem_t));
  new->data = strndup(key, strlen(key));
  new->head = NULL;
  new->tail = NULL;
  return new;
}

static void free_element(elem_t * elem)
{
  free(elem->data);
  free(elem);
}

int aky_key_init (void)
{
  if (hcreate_r (1000, &hash) == 0){
    fprintf (stderr,
             "[aky_converter] at %s,"
             "hash table allocation failed.",
             __FUNCTION__);
    return 1;
  }else{
    return 0;
  }
}

void aky_key_free (void)
{
  hdestroy_r (&hash);
}
char *aky_put_key(const char *type, int src, int dst, char *key, int n)
{
  //zeroe key
  bzero (key, n);

  char aux[100];
  bzero (aux, 100);
  snprintf(aux, 100, "%s#%d#%d", type, src, dst);
  ENTRY e, *ep = NULL;
  e.key = aux;
  e.data = NULL;

  hsearch_r (e, FIND, &ep, &hash);
  if (ep == NULL) {
    e.key = strdup(aux);
    e.data = malloc(sizeof(desc_t));
    ((desc_t *) e.data)->first = NULL;
    ((desc_t *) e.data)->last = NULL;
    ((desc_t *) e.data)->n = 0;
    hsearch_r (e, ENTER, &ep, &hash);
  }
  elem_t *new = new_element(src, dst, key, n);
  enqueue(ep->data, new);
  return key;
}

char *aky_get_key(const char *type, int src, int dst, char *key, int n)
{
  //zeroe key
  bzero (key, n);

  char aux[100];
  bzero (aux, 100);
  snprintf(aux, 100, "%s#%d#%d", type, src, dst);
  ENTRY e, *ep;
  e.key = aux;
  e.data = NULL;

  hsearch_r (e, FIND, &ep, &hash);
  if (ep == NULL) {
    fprintf (stderr,
             "[aky_converter] at %s (no queue), there is no key available\n"
             "[aky_converter] when type = %s, src = %d, dst = %d.\n",
             __FUNCTION__, type, src, dst);
    return NULL;
  }
  elem_t *elem = dequeue(ep->data);
  if (elem == NULL) {
    fprintf (stderr,
             "[aky_converter] at %s (no key), there is no key available\n"
             "[aky_converter] when type = %s, src = %d, dst = %d.\n",
             __FUNCTION__, type, src, dst);
    return NULL;
  }

  //copy key into output
  snprintf(key, n, "%s", elem->data);
  free_element(elem);
  return key;
}
