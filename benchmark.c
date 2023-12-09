#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "db.h"
#include "dict.h"
#define N_TEST_TIME 1000000

void _set(DB *db){
    char data[8];
    char key[8];
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i);
        sprintf(data, "%d", i);
        int state = set(db, key, data);
    }
}

void _get(DB *db){
    char key[8];
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i);
        char *value = get(db, key);
    }
}

void _del(DB *db){
    char key[8];
    for(int i = 0;i < N_TEST_TIME;i++){    
        sprintf(key, "%d", i);
        int state = del(db, key);
    }
}

double _duration(time_t start, time_t end){
    return (double)(end - start) / (CLOCKS_PER_SEC);
}

void benchmark_dict(){
    DB *db = DB_create(BUCKET_SIZE_LIST[15]);
    time_t start, end;
    double duration = 0; //s
    double average = 0; // us

    start = clock();
    _set(db);
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("Insert\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);

    start = clock();
    _get(db);
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("Get\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);

    start = clock();
    _del(db);
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("Delete\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);
    printf("\n");

    DB_free(db);
}

void benchmark_dlist(){
    DB *db = DB_create(BUCKET_SIZE_LIST[15]);
    char data[8];
    char key[8];
    time_t start, end;
    double duration = 0; // s
    double average = 0; // us

    start = clock();
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        sprintf(data, "%d", i);
        int state = lpush(db, key, data);
    }
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("lpush\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);

    start = clock();
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        sprintf(data, "%d", i);
        int state = rpush(db, key, data);
    }
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("rpush\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);

    start = clock();
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        int state = lpop(db, key);
    }
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("lpop\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);

    start = clock();
    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        int state = rpop(db, key);
    }
    end = clock();
    duration = _duration(start, end);
    average = duration / N_TEST_TIME * 1000000; // convert s to us
    printf("rpop\t%d\tdata", N_TEST_TIME);
    printf("\tTotal %f s\taverage %f us\n", duration, average);

    DB_free(db);
}

int main(){
    benchmark_dict();
    benchmark_dlist();
}