#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "datatype.h"
#include "status.h"
#include "murmurhash.h"
#include "str.h"
#include "dict.h"

#define MAX_DATA_LEN 100000

const size_t BUCKET_SIZE_LIST[] = {
    53,			97,			193,		389,		769,
	1543,		3079,		6151,		12289,		24593,
	49157,		98317,		196613,		393241,		786433,
	1572869,	3145739,	6291469,	12582917,	25165843,
	50331653,	100663319,	201326611 
};
const int BUCKET_SIZE_LIST_NUM = 23;

DictEntry* dictEntry_create_novalue(const char * const key){
    if(key == NULL) 
        return NULL;
    DictEntry *de = (DictEntry*)malloc(sizeof(DictEntry));
    size_t key_len = strlen(key);
    de->key = (char*)malloc(sizeof(char)*(key_len+1));
    strncpy(de->key, key, key_len+1);
    de->type = TYPE_NOVALUE;
    de->next = NULL;
    return de;
}

int dictEntry_set_val(DictEntry* de, DBobj* val){
    if(de == NULL || val == NULL) 
        return FAIL;
    de->type = val->type;
    de->v.val = val;
    return SUCCESS;
}

int dictEntry_set_string(DictEntry* de, const char * const source){
    if(de == NULL || source == NULL) 
        return FAIL;
    int state = SUCCESS;
    if(de->type != TYPE_NOVALUE) // free and override
        state = dictEntry_free_val(de);
    if(state == FAIL) 
        return FAIL;
    de->type = TYPE_STRING;
    size_t l1 = strlen(source);
    de->v.val = (char*)malloc(sizeof(char)*(l1+1));
    strncpy(de->v.val, source, l1+1); // strncpy problem: is l1 correct ? 
    return state;
}

int dictEntry_free_val(DictEntry *de){
    if(de == NULL) 
        return FAIL;
    int state = SUCCESS;
    // if no value
    if(de->type == TYPE_STRING){
        free(de->v.val);
    }else if(de->type == TYPE_LIST){ // TODO: free other types
        state = DBobj_free(de->v.val);
    }
    de->type = TYPE_NOVALUE;
    return state;
}

int dictEntry_free(DictEntry *de){
    if(de == NULL) 
        return FAIL;
    dictEntry_free_val(de);
    free(de->key);
    free(de);
    return SUCCESS;
}

DictEntryList* dictEntryList_create(){
    DictEntryList *delist = (DictEntryList*)malloc(sizeof(DictEntryList));
    delist->head = NULL;
    return delist;
}

int dictEntryList_free(DictEntryList *delist){
    if(delist == NULL)
        return FAIL;
    if(delist->head == NULL){
        free(delist);
        return SUCCESS;
    } 
    DictEntry *current = delist->head;
    while(current != NULL){
        DictEntry *next = current->next;
        dictEntry_free(current);
        current = next;
    }
    free(delist);

    return SUCCESS;
}

// we have to malloc de and pass to function
int dictEntryList_insert(DictEntryList * delist, DictEntry *de){
    if(delist == NULL || de == NULL)
        return FAIL;
    if(delist->head == NULL){
        de->next = NULL;
        delist->head = de;
    }else{
        de->next = delist->head;
        delist->head = de;
    }
    // printf("insert (%s, %s) to list\n", key, data);
    return SUCCESS;
}

DictEntry* dictEntryList_find(DictEntryList * delist, const char * const key){
    if(delist == NULL || key == NULL)
        return NULL;
    DictEntry *current = delist->head;
    while (current != NULL){
        if(strcmp(current->key, key) == 0)
            return current;
        current = current->next;
    }
    return NULL;
}

/*
    在redisDb當中，預設每一個value都是一個redisObject，也就是說 
    如果要刪除一個key-value pair，釋放key的時候呼叫dictGetVal，
    而此一函式會取出dictEntry->v.val

    在其他情況下，才會呼叫dictGetSignedIntegerVal(dictEntry->v.s64)，
    等其他union成員
*/

/*
    UnlinkFind: remove and return Node from DictEntryList to free key and value in DictEntry
*/
DictEntry* dictEntryList_UnlinkFind(DictEntryList * delist, const char * const key){
    if(delist->head == NULL)
        return NULL;
    DictEntry *prev = delist->head;
    DictEntry *current = delist->head;
    while (current != NULL){
        // found
        if(strcmp(current->key, key) == 0){ 
            if(current == delist->head && current->next == NULL) // head is target & next is NULL
                delist->head = NULL;
            else if(current == delist->head && current->next != NULL)
                delist->head = current->next;
            else 
                prev->next = current->next;
            
            return current;
        }
        // not fonud
        prev = current;
        current = current->next;
    }
    return NULL;
}

// it's general delete, when de->v.val is used
// TODO: if free `de->v.val` is not needed, call another function (???)
int dictEntryList_delete(DictEntryList * delist, const char * const key){ 
    if(delist->head == NULL)
        return FAIL;
    DictEntry *de = dictEntryList_UnlinkFind(delist, key);
    if(de == NULL)
        return FAIL;
    dictEntry_free(de);
    
    return SUCCESS;
}

uint32_t 
hashFunction(const char * const key){
    uint32_t seed = 0;
    uint32_t hash = murmurhash(key, (uint32_t) strlen(key), seed); // 0xb6d99cf8
    return hash;
}

Dict* dict_create(size_t bucket_size){
    Dict* dict = (Dict*)malloc(sizeof(Dict));
    dict->bucket = (DictEntryList**)malloc(sizeof(DictEntryList*)*bucket_size);
    for(int idx = 0;idx < bucket_size;idx++){
        (dict->bucket)[idx] = dictEntryList_create();
    }
    dict->n_entry = 0;
    dict->n_bucket = bucket_size;
    return dict;
}

int dict_free(Dict *dict){
    if(dict == NULL) 
        return FAIL;
    int state = SUCCESS;
    for(int idx = 0;idx < dict->n_bucket;idx++){
        DictEntryList* delist = (dict->bucket)[idx];
        state = dictEntryList_free(delist);
        if(state == FAIL)
            return FAIL;
    }
    free(dict->bucket);
    free(dict);

    return SUCCESS;
}

/*  
    set new key and return dictEntry if key does not exist ; 
    otherwise, return existed dictEntry
*/
DictEntry* dict_set_key(Dict *dict, const char * const key){
    if(key == NULL || dict == NULL)
        return NULL;
    uint32_t hash = hashFunction(key);
    int idx = hash % dict->n_bucket;
    // store to bucket
    DictEntryList *delist = (dict->bucket)[idx];
    DictEntry *de = dictEntryList_find(delist, key);
    if(de == NULL){
        de = dictEntry_create_novalue(key);
        int state = dictEntryList_insert(delist, de);
        if(state == FAIL)
            return NULL;
        dict->n_entry++;
        return de;
    }else
        return de;
}

// refactor....
// int dict_set_val(Dict *dict, const char * const key, void *val){ // Is it usefull ? 
//     // getHash: hashfunction
//     uint32_t hash = hashFunction(key);
//     // mode size
//     int idx = hash % dict->n_bucket;
//     // store to bucket
//     DictEntryList *delist = (dict->bucket)[idx];
//     DictEntry *de = dictEntryList_find(delist, key);
//     if(de != NULL){
//         // !!!
//         if(de->type == TYPE_STRING){
//             size_t l1 = strlen(de->v.val);
//             size_t l2 = strlen((char*)val);
//             if(l1 < l2){
//                 free(de->v.val);
//                 de->v.val = (char*)malloc(sizeof(char)*l2);
//             }
//             strncpy(de->v.val, (char*)val, l2);
//             // strcpy(de->v.val, (char*)val);
//         }else{
//             dictEntry_free_val(de);
//             size_t l2 = strlen((char*)val);
//             de->v.val = (char*)malloc(sizeof(char)*l2);
//             strncpy(de->v.val, (char*)val, l2);
//             // strcpy(de->v.val, (char*)val);
//         }
//         return SUCCESS;
//     }else{
//         int state = dictEntryList_insert(delist, de);
//         assert(state == SUCCESS);
//         dict->n_entry++;
//         return state;
//     }
// }

DictEntry *dict_get(Dict *dict, const char * const key){
    if(key == NULL) 
        return NULL;
    // getHash: hashfunction
    uint32_t hash = hashFunction(key);
    // mode size
    int idx = hash % dict->n_bucket;
    DictEntryList *delist = (dict->bucket)[idx];
    DictEntry *de = dictEntryList_find(delist, key);

    return de;
}

int dict_del(Dict *dict, const char * const key){
    if(key == NULL) 
        return FAIL;
    uint32_t hash = hashFunction(key);
    int idx = hash % dict->n_bucket;

    DictEntryList *delist = (dict->bucket)[idx];
    DictEntry *de = dictEntryList_UnlinkFind(delist, key); // Check: why not use dictEntryList_delete
    if(de == NULL)
        return FAIL;
    dictEntry_free(de);
    dict->n_entry--;

    return SUCCESS;
}