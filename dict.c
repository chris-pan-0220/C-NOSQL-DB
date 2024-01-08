#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "datatype.h"
#include "status.h"
#include "murmurhash.h"
#include "str.h"
#include "dict.h"
#include "event.h"

#define MAX_DATA_LEN 100000
#define LF_MAX 0.20
#define LF_MIN 0.10

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

int dictEntry_set_signedint(DictEntry* de, int64_t val){
    if(de == NULL) 
        return FAIL;
    int state = SUCCESS;
    if(de->type != TYPE_NOVALUE) // free and override. it's OK ?
        state = dictEntry_free_val(de);
    de->type = TYPE_SIGNED_INT64;
    de->v.s64 = val;
    return state;
}

int dictEntry_set_unsignedint(DictEntry* de, uint64_t val){
    if(de == NULL) 
        return FAIL;
    int state = SUCCESS;
    if(de->type != TYPE_NOVALUE) // free and override. it's OK ?
        state = dictEntry_free_val(de);
    de->type = TYPE_UNSIGNED_INT64;
    de->v.u64 = val;
    return state;
}

int dictEntry_free_val(DictEntry *de){
    if(de == NULL) 
        return FAIL;
    int state = SUCCESS;
    // if no value
    if(de->type == TYPE_STRING){
        free(de->v.val);
    }else if(de->type == TYPE_LIST || de->type == TYPE_DICT){
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
    
    // malloc bucket 0
    (dict->bucket)[0] = (DictEntryList**)malloc(sizeof(DictEntryList*)*bucket_size);
    (dict->n_bucket)[0] = bucket_size;

    // malloc bucket 1
    for(int i = 0;i < BUCKET_SIZE_LIST_NUM;i++){
        if(BUCKET_SIZE_LIST[i] > bucket_size || i == BUCKET_SIZE_LIST_NUM - 1){
            (dict->bucket)[1] = (DictEntryList**)malloc(sizeof(DictEntryList*)*BUCKET_SIZE_LIST[i]);
            (dict->n_bucket)[1] = BUCKET_SIZE_LIST[i];
            break;
        }
    }
    
    for(int idx = 0;idx < (dict->n_bucket)[0];idx++){
        (dict->bucket)[0][idx] = dictEntryList_create();
    }

    for(int idx = 0;idx < (dict->n_bucket)[1];idx++){
        (dict->bucket)[1][idx] = dictEntryList_create();
    }

    (dict->n_entry)[0] = 0;
    (dict->n_entry)[1] = 0;

    

    // register watcher
    dict->watcher = (ev_migrate*)malloc(sizeof(ev_migrate));
    dict->watcher->dict = dict;

    

    ev_idle_init(&(dict->watcher->w), migrate_cb);
    
    // get loop
    if(loop == NULL) // 提早啟動loop???
        initialize_loop();
    
    ev_idle_start(loop, &(dict->watcher->w));
    
    return dict;
}

int dict_free(Dict *dict){
    if(dict == NULL) 
        return FAIL;
    int state = SUCCESS;

    // 
    for(int idx = 0;idx < (dict->n_bucket)[0];idx++){
        DictEntryList* delist = (dict->bucket)[0][idx];
        state = dictEntryList_free(delist);
        if(state == FAIL)
            return FAIL;
    }
    free((dict->bucket)[0]);

    for(int idx = 0;idx < (dict->n_bucket)[1];idx++){
        DictEntryList* delist = (dict->bucket)[1][idx];
        state = dictEntryList_free(delist);
        if(state == FAIL)
            return FAIL;
    }
    free((dict->bucket)[1]);

    free(dict);

    return SUCCESS;
}

void dict_info(Dict *dict){
    printf("Used table: %d\n", dict->used);
    printf("In migration: %d\n", dict->migrating);
    if(dict->migrating){
        printf("migration index: %d\n", dict->migrate_idx);
    }
    printf("table 0 n_bucket: %ld\n", (dict->n_bucket)[0]);
    printf("table 0 n_entry: %ld\n", (dict->n_entry)[0]);
    printf("table 1 n_bucket: %ld\n", (dict->n_bucket)[1]);
    printf("table 1 n_entry: %ld\n", (dict->n_entry)[1]);
}

/*  
    set new key and return dictEntry if key does not exist ; 
    otherwise, return existed dictEntry
*/
DictEntry* dict_set_key(Dict *dict, const char * const key){
    if(key == NULL || dict == NULL)
        return NULL;
    uint32_t hash = hashFunction(key);

    int table_idx = 0;
    if(dict->migrating){
        table_idx = !dict->used; // 0 to 1, 1 to 0
    }else{ // not migrate 
        table_idx = dict->used;
    }

    int idx = hash % (dict->n_bucket)[table_idx];
    // store to bucket
    DictEntryList *delist = (dict->bucket)[table_idx][idx];
    DictEntry *de = dictEntryList_find(delist, key);
    if(de == NULL){
        de = dictEntry_create_novalue(key);
        int state = dictEntryList_insert(delist, de);
        if(state == FAIL)
            return NULL;
        (dict->n_entry)[table_idx]++;
        return de;
    }else
        return de;
}

DictEntry *dict_get(Dict *dict, const char * const key){
    if(key == NULL) 
        return NULL;
    // getHash: hashfunction
    uint32_t hash = hashFunction(key);

    // not migrate: search in used table
    if(dict->migrating){
        for(int table_idx = 0;table_idx <= 1;table_idx++){
            int idx = hash % (dict->n_bucket)[table_idx];
            DictEntryList *delist = (dict->bucket)[table_idx][idx];
            DictEntry *de = dictEntryList_find(delist, key);
            if(de != NULL)
                return de;
        }
        return NULL;
    }else{ // not migrating
        int table_idx = dict->used;
        int idx = hash % (dict->n_bucket)[table_idx];
        DictEntryList *delist = (dict->bucket)[table_idx][idx];
        DictEntry *de = dictEntryList_find(delist, key);
        return de;
    }
}

int dict_del(Dict *dict, const char * const key){
    if(key == NULL) 
        return FAIL;
    uint32_t hash = hashFunction(key);

    if(dict->migrating){
        for(int table_idx = 0;table_idx <= 1;table_idx++){
            int idx = hash % (dict->n_bucket)[table_idx];

            DictEntryList *delist = (dict->bucket)[table_idx][idx];
            DictEntry *de = dictEntryList_UnlinkFind(delist, key); // Check: why not use dictEntryList_delete
            
            if(de == NULL)
                continue;

            dictEntry_free(de);
            (dict->n_entry)[table_idx]--;

            return SUCCESS;
        }
        return FAIL;
    }else{
        int table_idx = dict->used;
        int idx = hash % (dict->n_bucket)[table_idx];

        DictEntryList *delist = (dict->bucket)[table_idx][idx];
        DictEntry *de = dictEntryList_UnlinkFind(delist, key); // Check: why not use dictEntryList_delete
        
        if(de == NULL)
            return FAIL;

        dictEntry_free(de);
        (dict->n_entry)[table_idx]--;

        return SUCCESS;
    }
    
}

void migrate_cb(struct ev_loop *loop, ev_idle *w, int revent){
    struct ev_migrate *watcher = (struct ev_migrate*)w; // TODO: add watcher
    Dict *dict = watcher->dict;

    // needn't to migrate
    if(!dict->migrating){ // assmue another bucket is clean
        
        int table_idx = dict->used;
        double lf = (double)(dict->n_entry)[table_idx] / (dict->n_bucket)[table_idx];

        if(lf > LF_MAX){

            if((dict->n_bucket)[table_idx] >= BUCKET_SIZE_LIST[BUCKET_SIZE_LIST_NUM-1])
                return;

            // create a larger bucket
            if((dict->n_bucket)[!table_idx] <= (dict->n_bucket)[table_idx]){
                for(int i = 0;i < BUCKET_SIZE_LIST_NUM;i++){
                    if(BUCKET_SIZE_LIST[i] > (dict->n_bucket)[table_idx]){
                        // free bucket first, assume that it is empty in dictEntryList
                        for(int idx = 0;idx < (dict->n_bucket)[!table_idx];idx++){
                            dictEntryList_free((dict->bucket)[!table_idx][idx]);
                        }
                        free((dict->bucket)[!table_idx]);

                        (dict->n_bucket)[!table_idx] = BUCKET_SIZE_LIST[i];
                        (dict->n_entry)[!table_idx] = 0;
                        (dict->bucket)[!table_idx] = (DictEntryList**)malloc(sizeof(DictEntryList*)*BUCKET_SIZE_LIST[i]);
                        for(int idx = 0;idx < BUCKET_SIZE_LIST[i];idx++){
                            (dict->bucket)[!table_idx][idx] = dictEntryList_create();
                        }
                    }
                }
            }else{
                // ignore. It has larger bucket.
            }

            dict->migrate_idx = 0;
            dict->migrating = 1;
            
        }else if(lf < LF_MIN){

            if((dict->n_bucket)[table_idx] <= BUCKET_SIZE_LIST[0])
                return;

            // create a smaller bucket
            // dictEntryList不能被free
            if((dict->n_bucket)[!table_idx] >= (dict->n_bucket)[table_idx]){

                for(int i = BUCKET_SIZE_LIST_NUM - 1;i >= 0 ;i--){
                    if(BUCKET_SIZE_LIST[i] < (dict->n_bucket)[table_idx]){
                        // free bucket first, assume that it is empty in dictEntryList
                        for(int idx = 0;idx < (dict->n_bucket)[!table_idx];idx++){
                            dictEntryList_free((dict->bucket)[!table_idx][idx]);
                        }
                        free((dict->bucket)[!table_idx]);

                        (dict->n_bucket)[!table_idx] = BUCKET_SIZE_LIST[i];
                        (dict->n_entry)[!table_idx] = 0;
                        (dict->bucket)[!table_idx] = (DictEntryList**)malloc(sizeof(DictEntryList*)*BUCKET_SIZE_LIST[i]);
                        for(int idx = 0;idx < BUCKET_SIZE_LIST[i];idx++){
                            (dict->bucket)[!table_idx][idx] = dictEntryList_create();
                        }
                    }
                }
            }else{
                // ignore. It has smaller bucket.
            }

            dict->migrate_idx = 0;
            dict->migrating = 1;

        }else{
            // needn't to migrate
            return;
        }
    }else{ 
        int table_idx = dict->used;
        while(dict->migrate_idx < (dict->n_bucket)[table_idx] && ((dict->bucket)[table_idx][dict->migrate_idx])->head == NULL){
            dict->migrate_idx++;
        }
        // check migrate 是否完成
        if(dict->migrate_idx >= (dict->n_bucket)[table_idx]){// End migration. It should be `==`
            dict->migrate_idx = 0;
            dict->migrating = 0;
            dict->used = !dict->used;
            return;
        }

        // migrate an element
        // unlink a node from list
        DictEntryList *delist = (dict->bucket)[table_idx][dict->migrate_idx];
        DictEntry *de = delist->head;
        delist->head = delist->head->next;
        // decrement entry number
        (dict->n_entry)[table_idx]--;
            
        // get hash of key
        uint32_t hash = hashFunction(de->key);
        // get idx
        int idx = hash % (dict->n_bucket)[!table_idx];
        // insert to another bucket
        dictEntryList_insert((dict->bucket)[!table_idx][idx], de);
        // increment entry number
        (dict->n_entry)[!table_idx]++;
    }
}

/**


typedef struct Dict{ 
    DictEntryList **bucket[2];
    size_t n_bucket[2];
    size_t n_entry[2];
    // 用default size去初始化
    int used; // default 0 ; otherwise 1
    int migrating; // default 0
    int migrate_idx; // default 0
    ev_migrate *watcher; // store callback
} Dict;

一開始dict直接創建兩個不同長度的bucket(53, 97)
每一個dict在創建的時候註冊一個idle，當event loop進入idle狀態，
if load factor > 0.7, 持續擴增表的大小直到load factor不再大於0.7, 或是已經最大
    used變數紀錄目前使用的表
    從migrate的index, 取一個dictListEntry遷移到另外一個表(比較大的)
else load factor < 0.1, 持續縮小表的大小直到load factor不再小於0.1, 或是已經最小
    used變數紀錄目前使用的表
    從migrate的index, 取一個dictListEntry遷移到另外一個表(比較小的)

migrate流程

甚麼時候完成migrate: 當migrate_idx[used] == n_bucket[used], 完成migrate
migrate_idx設為0
used切換

一次遷移一個元素(for a key)
*/