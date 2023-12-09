#ifndef DB_H 
#define DB_H

typedef struct DB{
    struct Dict *dict; // change to **dict if implemented cluster mode
} DB;

/* DB API */
DB *DB_create(size_t bucket_size);
int DB_free(DB *db);

// only availible on `string`
int set(DB *db, const char * const key, const char * const val);
char* get(DB *db, const char * const key);
int del(DB *db, const char *const key);

// only availible on `dlist`
int lpush(DB *db, const char * const key, const char * const val);
int lpop(DB *db, const char * const key);
int rpush(DB *db, const char * const key, const char * const val);
int rpop(DB *db, const char * const key);
int llen(DB *db, const char * const key);
int lrange(DB *db, const char * const key, int left, int right); // problem: int left, int right

// only availible on `set`
// only availible on `dict`

#endif