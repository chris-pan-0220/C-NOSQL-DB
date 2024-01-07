#ifndef DB_H 
#define DB_H
#include <ev.h>
#include <stdint.h>

typedef struct DB{
    struct Dict *dict; // change to **dict if implemented cluster mode
    struct Dict *key_time;  // keys that will expire
} DB;

/* DB API */
struct ev_loop* initialize_loop();
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
int hset(DB *db, const char * const key, const char * const field, const char * const val); // key field value;
char* hget(DB *db, const char * const key, const char * const field);
int hdel(DB *db, const char * const key, const char * const field);
int expire(DB *db, const char * const key, uint64_t time); // set ket expire
static void expire_cb(struct ev_loop *loop, ev_timer *w, int revent);

// TODO:
// expire

#endif