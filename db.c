#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "datatype.h"
#include "status.h"
#include "db.h"
#include "dlist.h"
#include "dbobj.h"
#include "str.h"
#include "dict.h"



DB *DB_create(size_t bucket_size){
    DB *db = (DB*)malloc(sizeof(DB));
    db->dict = dict_create(bucket_size);
    return db;
}
// void free_db(DB *db);
int DB_free(DB *db){
    dict_free(db->dict);
    free(db);
    return SUCCESS;
}



// only availible on string
int set(DB *db, const char * const key, const char * const val){
    // new key: set key and return new de
    // existed key: return existed de
    DictEntry *de = dict_set_key(db->dict, key);
    // it's ok to override another data type (dlist, set...)
    int state = dictEntry_set_string(de, val); // check: copy val to de->v.val !
    return state;
}

char* get(DB *db, const char * const key){ // problem: return const char* ???
    DictEntry *de = dict_get(db->dict, key);
    if(de == NULL) 
        return NULL;
    if(de->type != TYPE_STRING)
        return NULL; // TODO: how to return type error ?
    return de->v.val; // problem: 字串真的是存在v.val嗎
}

int del(DB *db, const char *const key){
    int state = dict_del(db->dict, key);
    return state;
}

// only availible on `dlist`
// push a string to dlist
int lpush(DB *db, const char * const key, const char * const val){
    if(db == NULL || key == NULL || val == NULL)
        return FAIL;
    // new key: set key and return new de
    // existed key: return existed de
    DictEntry *de = dict_set_key(db->dict, key);
    if(de->type == TYPE_NOVALUE){    
        DBobj *obj_str = DBobj_create(TYPE_STRING); // create an empty string
        string_set((String*)obj_str->value, val);

        DBobj *obj_list = DBobj_create(TYPE_LIST); // create an empty list
        dlist_push_front((Dlist*)DBobj_get_val(obj_list), obj_str);

        return dictEntry_set_val(de, obj_list);
    }else if(de->type == TYPE_LIST){
        DBobj *obj_str = DBobj_create(TYPE_STRING); // create empty string
        string_set((String*)obj_str->value, val);
        return dlist_push_front((Dlist*)DBobj_get_val(de->v.val), obj_str);// de->v.val is DBobj, dlist
    }else 
        return FAIL;
}

int lpop(DB *db, const char * const key){
    DictEntry *de = dict_get(db->dict, key);
    if(de->type != TYPE_LIST)
        return FAIL;
    int state = dlist_pop_front((Dlist*)DBobj_get_val(de->v.val)); // de->v.val is DBobj, dlist
    return state;
}

int rpush(DB *db, const char * const key, const char * const val){
    // new key: set key and return new de
    // existed key: return existed de
    DictEntry *de = dict_set_key(db->dict, key);
    DBobj *obj_str = DBobj_create(TYPE_STRING); // create empty string
    string_set((String*)obj_str->value, val);
    if(de->type == TYPE_NOVALUE){    
        DBobj *obj_list = DBobj_create(TYPE_LIST);
        dlist_push_tail((Dlist*)DBobj_get_val(obj_list), obj_str);
        return dictEntry_set_val(de, obj_list);
    }else if(de->type == TYPE_LIST){
        return dlist_push_tail((Dlist*)DBobj_get_val(de->v.val), obj_str);// de->v.val is DBobj, dlist
    }else 
        return FAIL;
}

int rpop(DB *db, const char * const key){
    DictEntry *de = dict_get(db->dict, key);
    if(de->type != TYPE_LIST)
        return FAIL;
    int state = dlist_pop_tail((Dlist*)DBobj_get_val(de->v.val)); // de->v.val is DBobj, dlist
    return state;
}

int llen(DB *db, const char * const key){
    DictEntry *de = dict_get(db->dict, key);
    if(de->type != TYPE_LIST) // how to thow error here ??? 
        return -1;
    return ((Dlist*)DBobj_get_val(de->v.val))->len;
}

int lrange(DB *db, const char * const key, int left, int right){ // problem: int left, int right (out of range)
    if(db == NULL || key == NULL) 
        return FAIL;
    DictEntry *de = dict_get(db->dict, key);
    if(de->type != TYPE_LIST) // how to thow error here ??? 
        return FAIL;
    // Dlist *dlist = ((DBobj*)de->v.val)->value; // 易忘記
    Dlist *dlist = (Dlist*)DBobj_get_val(de->v.val); // de->v.val is DBobj, use DBobj get
    if(left < 0)
        left = dlist->len + left; // ex: 10 + (-1), the last element
    if(right < 0)
        right = dlist->len + right; // ex: 10 + (-1), the last element
    if(!(0 <= left && right < dlist->len && left <= right)) // how to throw error
        return FAIL;
    
    Dnode *ptr = dlist->head;
    int pos = 0;
    while(pos < left){
        ptr = ptr->next;
        pos++;
    } // Now, pos = left
    while(pos <= right){
        printf("%d) %s\n", pos - left + 1, ((String*)DBobj_get_val(ptr->value))->value);// ptr->value is DBobj
        pos++;
        ptr = ptr->next;
    }

    return SUCCESS;
}