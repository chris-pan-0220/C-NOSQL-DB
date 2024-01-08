#ifndef DICT_H
#define DICT_H
#include <ev.h>
#include <stdint.h>
#include "dbobj.h"
#include "event.h"

extern const size_t BUCKET_SIZE_LIST[];
extern const int BUCKET_SIZE_LIST_NUM;

typedef struct DictEntry{
    char *key;
    union {
        void *val;
        uint64_t u64; // needed ?
        int64_t s64;  // store unsinged / signed integer directly
    } v;
    unsigned type;
    struct DictEntry *next;
} DictEntry;

// simple list for a buckets of dict
typedef struct DictEntryList{
    DictEntry *head;
} DictEntryList;

// typedef struct Dict{ 
//     DictEntryList **bucket;
//     size_t n_bucket;
//     size_t n_entry;
//     // enum{ FIRST, SECOND } used;
//     // int migrate_index;
// } Dict;

typedef struct Dict{ 
    DictEntryList **bucket[2];
    size_t n_bucket[2];
    size_t n_entry[2];
    // 用default size去初始化
    int used; // default 0 ; otherwise 1
    int migrating; // default 0
    int migrate_idx; // default 0
    struct ev_migrate *watcher; // store callback
} Dict;

typedef struct ev_migrate{
    ev_idle w;
    Dict *dict;
} ev_migrate;

/**
 * 影響範圍: 
 * n_bucket
 * n_entry
 * bucket
 * 
 * 


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

extern const size_t BUCKET_SIZE_LIST[]; // sizes of BUCKET, for increasing capacity of dict
extern const int BUCKET_SIZE_LIST_NUM;

DictEntry* dictEntry_create_novalue(const char * const key); // create with no type value
int dictEntry_set_val(DictEntry* de, DBobj* val); // general set. Turn to set DBobj ?
int dictEntry_set_string(DictEntry* de, const char * const source); 
// TODO: 
int dictEntry_set_signedint(DictEntry* de, int64_t val);
int dictEntry_set_unsignedint(DictEntry* de, uint64_t val);
// int dictEntry_set_list(DictEntry* de, ); // 
int dictEntry_free_val(DictEntry *de);
int dictEntry_free(DictEntry *de);
uint32_t 
hashFunction(const char * const key);
Dict* dict_create(size_t bucket_size);
int dict_free(Dict *dict);
void dict_info(Dict *dict);


DictEntry *dict_set_key(Dict *dict, const char * const key); // refactor (???)
// int dict_set_val(Dict *dict, const char * const key, void *val); // needed ? 
DictEntry *dict_get(Dict *dict, const char * const key);
int dict_del(Dict *dict, const char * const key);

// idle: migrate to larger / smaller dict 
void migrate_cb(struct ev_loop *loop, ev_idle *w, int revent); 

// TODO: 
// Dict* dict_rehashcheck(Dict *dict);
// void dict_info(Dict *dict);
// Origin: 
// HashTable* rehash_check(HashTable *hashTable);
// void hashtable_info(HashTable *hashTable);

DictEntryList* dictEntryList_create();
int dictEntryList_free(DictEntryList * delist);
int dictEntryList_insert(DictEntryList * delist, DictEntry *de);
DictEntry* dictEntryList_find(DictEntryList * delist, const char * const key);
DictEntry* dictEntryList_UnlinkFind(DictEntryList * delist, const char * const key);
// delete key from dict, and return dictEntry for free operation 

int dictEntryList_delete(DictEntryList * delist, const char * const key); // general delete

#endif