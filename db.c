#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <ev.h>
#include "datatype.h"
#include "status.h"
#include "db.h"
#include "dlist.h"
#include "dbobj.h"
#include "str.h"
#include "dict.h"

static struct ev_loop *loop = NULL; // Is static OK ?

DB *DB_create(size_t bucket_size){
    DB *db = (DB*)malloc(sizeof(DB));
    db->dict = dict_create(bucket_size);
    db->key_time = NULL;
    return db;
}
// void free_db(DB *db);
int DB_free(DB *db){
    dict_free(db->dict);
    if(db->key_time)
        dict_free(db->key_time);
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

int hset(DB *db, const char * const key, const char * const field, const char * const val){
    if(db == NULL || key == NULL || field == NULL || val == NULL)
        return FAIL;
    
    // create a dict 
    DictEntry *de = dict_set_key(db->dict, key); // it's novalue-type
    // 原生 string
    if(de->type == TYPE_NOVALUE){ // create new dict
        DBobj *obj_dict = DBobj_create(TYPE_DICT);
        // set key - `field`
        DictEntry *de_ = dict_set_key((Dict *)DBobj_get_val(obj_dict), field);
        // set value
        dictEntry_set_string(de_, val); // TODO: state
        // attach new dict to db
        return dictEntry_set_val(de, obj_dict);
    }else if(de->type == TYPE_DICT){// dict has existed. `de->v.val` is dict
        // set key 
        DictEntry *de_ = dict_set_key((Dict *)DBobj_get_val(de->v.val), field);
        // set value
        return dictEntry_set_string(de_, val);
    }else 
        return FAIL;
}
char* hget(DB *db, const char * const key, const char * const field){
    if(db == NULL || key == NULL || field == NULL)
        return NULL;

    DictEntry *de = dict_get(db->dict, key);
    if(de->type == TYPE_DICT){
        DictEntry *de_ = dict_get((Dict *)DBobj_get_val(de->v.val), field);
        assert(de_->type == TYPE_STRING);
        return de_->v.val; // TODO: 統一string儲存
    }else
        return NULL;
}
int hdel(DB *db, const char * const key, const char * const field){
    if(db == NULL || key == NULL || field == NULL)
        return FAIL;

    DictEntry *de = dict_get(db->dict, key); // it's novalue-type
    if(de->type == TYPE_DICT)
        return dict_del((Dict *)DBobj_get_val(de->v.val), field);
    else
        return FAIL;
}

struct ev_loop* initialize_loop() {
    if (loop == NULL) {
        loop = ev_loop_new(EVFLAG_AUTO);
    }
    return loop;
}

struct expire_timer{
    struct ev_timer w;
    DB *db;
    char *key; // TODO:
};

int expire(DB *db, const char * const key, uint64_t time){ // expire time (sec)
    if(db == NULL || key == NULL || time <= 0)
        return FAIL;
    
    if(db->key_time == NULL)
        db->key_time = dict_create(BUCKET_SIZE_LIST[0]);
    // set key
    DictEntry *de = dict_set_key(db->key_time, key); // set unsigned int
    // set value to novalue-type
    if(de->type == TYPE_NOVALUE || de->type == TYPE_UNSIGNED_INT64){
        dictEntry_set_unsignedint(de, time);
        // set timer 
        // create watcher 
        struct expire_timer *timer = (struct expire_timer*)malloc(sizeof(struct expire_timer));
        timer->db = db;
        // timer->key = key; // Is it OK ?
        timer->key = (char*)malloc(sizeof(char)*64); // 64
        strncpy(timer->key, key, 64); // Is it OK ?
        ev_timer_init(&timer->w, expire_cb, time, 0.); // 0: no repeat
        ev_timer_start(loop, &timer->w);
        return SUCCESS;
    }else 
        return FAIL;
}

//
static void expire_cb(struct ev_loop *loop, ev_timer *w, int revent){
    // delete key, value pair from db->dict
    // delete that key from db->key->time
    struct expire_timer *timer = (struct expire_timer*)w;
    dict_del(timer->db->dict, timer->key); // expired key, value pair
    // null key ?
    dict_del(timer->db->key_time, timer->key);
    free(timer->key); // BAD ? 
    free(timer);
}