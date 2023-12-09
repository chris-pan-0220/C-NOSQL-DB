
#ifndef DLIST_H
#define DLIST_H

#include "dbobj.h"

/*
  DBobj *value: create obj before calling push function
*/
typedef struct Dnode{
    DBobj *value; 
    struct Dnode *prev;
    struct Dnode *next;
} Dnode;

typedef struct Dlist{
    struct Dnode *head;
    struct Dnode *tail;
    int len;
} Dlist;

Dlist* dlist_create();
int dlist_free(Dlist *list);
int dlist_push_front(Dlist *list, DBobj *val);
int dlist_pop_front(Dlist *list);
int dlist_push_tail(Dlist *list, DBobj *val);
int dlist_pop_tail(Dlist *list);

#endif