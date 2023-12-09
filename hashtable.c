// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <assert.h>

// #include "hashtable.h"
// #include "murmurhash.h"
// #define LOAD_FACTOR 1

// const size_t BUCKET_SIZE_LIST[] = {
//     53,			97,			193,		389,		769,
// 	1543,		3079,		6151,		12289,		24593,
// 	49157,		98317,		196613,		393241,		786433,
// 	1572869,	3145739,	6291469,	12582917,	25165843,
// 	50331653,	100663319,	201326611 
// };
// const int BUCKET_SIZE_LIST_NUM = 23;

// HashTable* hashtable_create(size_t bucket_size){
//     HashTable *hashtable = (HashTable*)malloc(sizeof(HashTable));
//     hashtable->buckets = (List**)malloc(sizeof(List*)*bucket_size);
//     hashtable->size = bucket_size;
//     // init buckets
//     for(int i = 0;i < bucket_size;i++) 
//         (hashtable->buckets)[i] = create_list();
//     hashtable->n_data = 0;
//     return hashtable;
// }

// void del_hashtable(HashTable* hashTable){
//     for(int i = 0;i < hashTable->size;i++){
//         del_list((hashTable->buckets)[i]);
//     }
//     free(hashTable->buckets);
//     free(hashTable);
// }

// uint32_t hashFunction(char* key){
//     uint32_t seed = 0;
//     uint32_t hash = murmurhash(key, (uint32_t) strlen(key), seed); // 0xb6d99cf8
//     return hash;
// }

// // insert new (key, value) pair / update value
// int set(HashTable *hashtable, char *key, char *data){
//     // getHash: hashfunction
//     uint32_t hash = hashFunction(key);
//     // mode size
//     int idx = hash % hashtable->size;
//     // store to bucket
//     Node *node = find((hashtable->buckets)[idx], key);
//     if(node != NULL){
//         size_t l1 = strlen(data), l2 = strlen(node->data); //to-do: optimal
//         if(l1 > l2){ 
//             // re-malloc
//             free(node->data);
//             node->data = (char*)malloc(sizeof(char)*(l1+1));
//         }
//         strcpy(node->data, data);
//     }else{
//         int state = insert((hashtable->buckets)[idx], key, data);
//         assert(state == SUCCESS);
        
//         hashtable->n_data++;
//         return state;
//     }
// }

// char* get(HashTable *hashtable, char *key){ // const char* ???
//     uint32_t hash = hashFunction(key);
//     int idx = hash % hashtable->size;

//     Node *node = find((hashtable->buckets)[idx], key);
//     if(node == NULL)
//         return NULL;
//     else
//         return node->data;
// }

// int del(HashTable *hashtable, char *key){
//     uint32_t hash = hashFunction(key);
//     int idx = hash % hashtable->size;

//     List *list = (hashtable->buckets)[idx];
//     Node *node = find(list, key);
//     // assert(node != NULL);
//     if(node == NULL) 
//         return FAIL;

//     int state = delete(list, key);
//     if(state == SUCCESS) hashtable->n_data--;
//     return state;
// }

// /************************** utility function *************************/

// HashTable* rehash_check(HashTable *hashTable){
//     // printf("rehash check: size = %d,  data = %d\n", hashTable->size, hashTable->n_data);
//     // assert(hashTable->size > 0);
//     if(hashTable->n_data / (double) hashTable->size > LOAD_FACTOR){
//         int bucket_size = hashTable->size;
//         int new_bucket_size = 0;
//         for(int i = 0;i < BUCKET_SIZE_LIST_NUM;i++){
//             if(BUCKET_SIZE_LIST[i] > bucket_size) {
//                 new_bucket_size = BUCKET_SIZE_LIST[i];
//                 break;
//             }
//         }
//         if(new_bucket_size == 0) return NULL; // 超過大小上限就不會再rehash了
//         // create new hashtable
//         HashTable *new_hashtable = create_hashtable(new_bucket_size);
//         // move elements to new hashtable
//         for(int i = 0;i < hashTable->size;i++){
//             // retreive
//             Node *current = (hashTable->buckets)[i]->head;
//             while(current != NULL){
//                 int state = set(new_hashtable, current->key, current->data);
//                 assert(state == SUCCESS);
//                 current = current->next;
//             }
//         }
//         // del hashtable
//         del_hashtable(hashTable); // 副作用
//         return new_hashtable;
//     }
//     return NULL;
// }

// void hashtable_info(HashTable *hashTable){
//     printf("Buckets:\t%ld\n", hashTable->size);
//     printf("Data:\t\t%ld\n\n", hashTable->n_data);
// }

