#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "status.h"
#include "dlist.h"
#include "dbobj.h"

// create a empty dlist 
Dlist* dlist_create(){
    Dlist *list = (Dlist*)malloc(sizeof(Dlist));
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
    return list;
}

// free a dlist
int dlist_free(Dlist *list){
    if(list == NULL)
        return FAIL;
    if(list->head == NULL){
        free(list);
        return SUCCESS;
    }
    while(list->head != NULL)
        dlist_pop_front(list);// TODO: state detection
    free(list);

    return SUCCESS;
}

/*
    problem:
    assume that val is DBobj, 
    when val is string ?
    註：dictEntry是默認有str的data type，可是DBobj也包裝了一層data type...
*/
int dlist_push_front(Dlist *list, DBobj *val){ 
    if(list == NULL || val == NULL) 
        return FAIL;
    Dnode *node = (Dnode*)malloc(sizeof(Dnode));
    if(node == NULL) // malloc fail 
        return FAIL;

    node->value = val; // problem: assume that val is DBobj, when val is string ?

    if(list->head == NULL){
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
    }else{
        node->next = list->head; // node->next = address of current head
        list->head = node; // list->head = address of new node
        node->prev = NULL;
        node->next->prev = node; // address of previous head->prev = address of node
    }
    list->len++;

    return SUCCESS;
}

int dlist_pop_front(Dlist *list){ // delete head
    if(list->head == NULL)
        return FAIL;
    // if next of head is NULL
    Dnode *head = list->head; // target
    
    if(list->head == list->tail){ // avoid tail access to the freed pointer `list->head`
        list->head = NULL;
        list->tail = NULL;
    }else{
        list->head = list->head->next; // new head
        list->head->prev = NULL;
    }
    list->len--;

    DBobj_free(head->value);// problem: Dnode可以直接儲存string嗎?還是string需要經過包裝
    free(head);

    return SUCCESS;
}

int dlist_push_tail(Dlist *list, DBobj *val){
    if(list == NULL || val == NULL) 
        return FAIL;
    Dnode *node = (Dnode*)malloc(sizeof(Dnode));
    if(node == NULL) // malloc fail 
        return FAIL;

    node->value = val;

    if(list->head == NULL){
        list->head = node;
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
    }else{
        node->prev = list->tail;
        list->tail = node;
        node->next = NULL;
        node->prev->next = node;
    }
    list->len++;

    return SUCCESS;
}

int dlist_pop_tail(Dlist *list){
    if(list->head == NULL)
        return FAIL;
    // if next of head is NULL
    Dnode *tail = list->tail; // target
    
    if(list->head == list->tail){ // avoid tail access to the freed pointer `list->head`
        list->head = NULL;
        list->tail = NULL;
    }else{
        list->tail = list->tail->prev; // new head
        list->tail->next = NULL;
    }
    list->len--;
        
    DBobj_free(tail->value);
    free(tail);

    return SUCCESS;
}


