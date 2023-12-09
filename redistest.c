#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>
#include <time.h>

#define N_TEST_TIME 1000000

void _set(redisContext *c){
    char data[8];
    char key[8];
    redisReply *reply;
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i);
        sprintf(data, "%d", i);
        // set 
        reply = redisCommand(c, "HSET db %s %s", key, data);
        freeReplyObject(reply);
    }
}

void _get(redisContext *c){
    char key[8];
    redisReply *reply;
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i);
        // get
        reply = redisCommand(c, "HGET db %s", key);
        freeReplyObject(reply);
    }
}


void _del(redisContext *c){
    char key[8];
    redisReply *reply;
    for(int i = 0;i < N_TEST_TIME;i++){    
        sprintf(key, "%d", i);
        // get
        reply = redisCommand(c, "HGET db %s", key);
        freeReplyObject(reply);
    }
}

double _duration(time_t start, time_t end){
    return (double)(end - start) / (CLOCKS_PER_SEC);
}

int benchmark(){
    redisContext *c;
    c = redisConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("error: %s\n", c->errstr);
        return 1;
    }

    time_t start, end;
    double duration = 0; //ms
    double average = 0; // ns
    start = clock();
    _set(c);
    end = clock();
    duration = _duration(start, end) * 1000;
    average = duration / N_TEST_TIME * 1000000; // convert ms to ns
    printf("Insert\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f ms\taverage %f ns\n", duration, average);

    start = clock();
    _get(c);
    end = clock();
    duration = _duration(start, end) * 1000;
    average = duration / N_TEST_TIME * 1000000; // convert ms to ns
    printf("Get\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f ms\taverage %f ns\n", duration, average);

    start = clock();
    _del(c);
    end = clock();
    printf("Delete\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f ms\taverage %f ns\n", duration, average);

    redisFree(c);
}

int main (int argc, char **argv) {
    benchmark();
    // redisReply *reply;
    // redisContext *c;

    // c = redisConnect("127.0.0.1", 6379);
    // if (c->err) {
    //     printf("error: %s\n", c->errstr);
    //     return 1;
    // }

    // /* PINGs */
    // // reply = redisCommand(c,"PING %s", "Hello World");
    // // printf("RESPONSE: %s\n", reply->str);
    // // reply = redisCommand(c, "SET db key1 %s", "Hello World");
    // // printf("RESPONSE: %s\n", reply->str);
    // // reply = redisCommand(c,"HGET db key1");
    // // printf("RESPONSE: %s\n", reply->str);
    // char data[8];
    // char key[8];
    // time_t start, end;
    // start = clock();
    // for(int i = 0;i < N_TEST_TIME;i++){
    //     sprintf(key, "%d", i);
    //     sprintf(data, "%d", i);
    //     // set 
    //     reply = redisCommand(c, "HSET db %s %s", key, data);
    //     freeReplyObject(reply);
    // }
    // end = clock();
    // printf("Insert\t%d\tdata", N_TEST_TIME);
    // printf("\tTotal %f ms\n", (double)(end - start)/CLOCKS_PER_SEC*1000);

    // start = clock();
    // for(int i = 0;i < N_TEST_TIME;i++){
    //     char key[8];
    //     sprintf(key, "%d", i);
    //     // get
    //     reply = redisCommand(c, "HGET db %s", key);
    //     freeReplyObject(reply);
    // }
    // end = clock();
    // printf("Get\t%d\tdata", N_TEST_TIME);
    // printf("\tTotal %f ms\n", (double)(end - start)/CLOCKS_PER_SEC*1000);

    // start = clock();
    // for(int i = 0;i < N_TEST_TIME;i++){
    //     char key[8];
    //     sprintf(key, "%d", i);
    //     reply = redisCommand(c, "HDEL db %s", key);
    //     freeReplyObject(reply);
    // }
    // end = clock();
    // printf("Delete\t%d\tdata", N_TEST_TIME);
    // printf("\tTotal %f ms\n", (double)(end - start)/CLOCKS_PER_SEC*1000);

    // redisFree(c);
}