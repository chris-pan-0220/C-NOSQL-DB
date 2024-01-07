
#ifndef SORTEDSET_H
#define SORTEDSET_H

#include "dbobj.h"

/*
  DBobj *value: create obj before calling push function
*/
typedef struct SortListNode{
    char *key;
    double score;
    struct SortListNode *next;
} SortListNode;

typedef struct SortList{
    struct SortListNode *head;
    int len;
} SortList;

SortList *sortList_create();
int sortList_insert(SortList *list, const char * const key, double val);
SortList *sortList_inter(SortList *source1, SortList *source2);
SortList *sortList_union(SortList *source1, SortList *source2);
int sortList_count_byscore(SortList *list, double left, double right);
int sortList_range_byindex(SortList *list, int left, int right);
int sortList_range_byscore(SortList *list, double left, double right);
int sortList_rank(SortList *list, const char * const key);
int sortList_delete_bykey(SortList *list, const char * const key); 
int sortList_delete_byscore(SortList *list, double left, double right); 
int sortList_free(SortList *list);

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
