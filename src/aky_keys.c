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
#include <aky.h>
#include <stdio.h>
#include <rastro.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>

static void *root = NULL;

typedef struct elem {
  struct elem *back;
  struct elem *forw;
  void *data;
}elem_t;

static elem_t *enqueue (elem_t *first, elem_t *new)
{
  if (first == NULL){
    new->back = NULL;
    new->forw = NULL;
    return new;
  }
  first->back = new;
  new->back = NULL;
  new->forw = first;
  return new;
}

static elem_t *dequeue (elem_t *first)
{
  elem_t *ret = first;
  while (ret){
    if (ret->forw == NULL) break;
    ret = ret->forw;
  }
  if (ret->back){
    ret->back->forw = NULL;
  }
  return ret;
}

char *aky_put_key (int src, int dst, char *key, int n)
{
  //get the dynar for src#dst
  ENTRY e, *ep;
  char aux[100];
  snprintf(aux, AKY_DEFAULT_STR_SIZE, "%d#%d", src, dst);
  e.key = aux;
  e.data = NULL;
  
  ep = hsearch (e, FIND);
  if (ep == NULL){
    ep = hsearch (e, ENTER);
  }

  //generate the key
  static unsigned long long counter = 0;
  snprintf(key, n, "%d%d%lld", src, dst, counter++);

  int len = strlen(key)+1;
  elem_t *elem = (elem_t*)malloc(sizeof(elem_t));
  elem->data = (char*)malloc(len*sizeof(char));
  elem->forw = NULL;
  elem->back = NULL;
  strncpy (elem->data, key, len);

  //put on queue
  ep->data = enqueue (ep->data, elem);
  while (elem){
    elem = elem->forw;
  }
  return key;
}

char *aky_get_key (int src, int dst, char *key, int n)
{
  ENTRY e, *ep;
  //get the dynar for src#dst
  char aux[100];
  snprintf(aux, AKY_DEFAULT_STR_SIZE, "%d#%d", src, dst);
  e.key = aux;
  ep = hsearch (e, FIND);
  elem_t *elem = dequeue (ep->data);
  snprintf (key, n, "%s", (char*)elem->data);
  if (ep->data == elem){
    ep->data = NULL;
  }
  free (elem->data);
  free (elem);
  return key;
}
