#ifndef DB_H 
#define DB_H
#include <ev.h>
#include <stdint.h>
#include "event.h"

typedef struct DB{
    struct Dict *dict; // change to **dict if implemented cluster mode
    struct Dict *key_time;  // keys that will expire
} DB;

typedef struct ev_expire{
    struct ev_timer w;
    DB *db;
    char *key; // TODO:
} ev_expire;

/* DB API */
// extern struct ev_loop *loop;
// struct ev_loop* initialize_loop();
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

// only availible on `dict`
int hset(DB *db, const char * const key, const char * const field, const char * const val); // key field value;
char* hget(DB *db, const char * const key, const char * const field);
int hdel(DB *db, const char * const key, const char * const field);
int hinfo(DB *db, const char * const key);
int expire(DB *db, const char * const key, uint64_t time);
static void expire_cb(struct ev_loop *loop, ev_timer *w, int revent);

// only availible on `set`
int zadd(DB *db, const char * const set_key, const char * const element_key, double val);
int zcard(DB *db, const char * const key);
int zcount(DB *db, const char * const key, double left, double right);
int zinterstore(DB *db, const char * const dest_key, const char * const source1_key, const char * const source2_key);
int zunionstore(DB *db, const char * const dest_key, const char * const source1_key, const char * const source2_key);
int zrange(DB *db, const char * const key, int left, int right);
int zrangebyscore(DB *db, const char * const key, double left, double right);
int zrank(DB *db, const char * const set_key, const char * const element_key);
int zrem(DB *db, const char * const set_key, const char * const element_key);
int zremrangebyscore(DB *db, const char * const key, double left, double right);
/**
 * 
 * ZADD 增加元素
 * ZCARD 查看元素數量
 * ZCOUNT 查詢範圍內成員的數量
 * ZINTERSTORE 把key交集, 分數加起來 Hashtable
 * ZUNIONSTORE 把key聯集, 分數加起來 Hashtable
 * ZRANGE 使用index範圍(按照分數排序)列出集合當中的元素 list
 * ZRANGEBYSCORE 使用score範圍列出集合當中的元素
 * ZRANK 用key查詢, 返回排名 
 * ZREM 用key, remove
 * ZREMRANGEBYSCORE 使用score範圍刪除範圍中的元素
*/
#endif