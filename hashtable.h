#include <stdint.h>

#include "list.h"
#include "status.h"

typedef struct HashTable{
    List **buckets;
    size_t size;
    size_t n_data;
} HashTable;

extern const size_t BUCKET_SIZE_LIST[];
extern const int BUCKET_SIZE_LIST_NUM;

uint32_t 
hashFunction(char* data);
HashTable* hashtable_create(size_t bucket_size);
void hashtable_free(HashTable* hashTable);
// only availible on `string`
int hashtable_set(HashTable *hashtable, const char * const key, void * val);
void* hashtable_get(HashTable *hashtable, const char * const key);
int hashtable_del(HashTable *hashtable, const char * const key);
// only availible on `list`

HashTable* rehash_check(HashTable *hashTable);
void hashtable_info(HashTable *hashTable);