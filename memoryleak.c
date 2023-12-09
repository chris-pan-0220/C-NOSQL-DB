#include <stdlib.h>
#include <stdio.h>

#include "db.h"

int main(){
    DB *db = DB_create(100);

    const int N_TEST_TIME = 100;
    char data[8];
    char key[8];

    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        sprintf(data, "%d", i);
        int state = lpush(db, key, data);
    }

    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        sprintf(data, "%d", i);
        int state = rpush(db, key, data);
    }

    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        int state = lpop(db, key);
        printf("Length in dlist (key is %d): %d\n", i%10, llen(db, key));
    }

    for(int i = 0;i < N_TEST_TIME;i++){
        sprintf(key, "%d", i%10);
        int state = rpop(db, key);
        printf("Length in dlist (key is %d): %d\n", i%10, llen(db, key));
    }

    DB_free(db);
}