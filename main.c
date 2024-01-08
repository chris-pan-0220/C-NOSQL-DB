#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <ev.h>
#include "db.h"
#include "dict.h"
#include "status.h"
#include "event.h"

#define BUFFER_SIZE 256 
#define N_MAX_TOKEN 16

DB *db;

void help(){
    printf("Usage: \n");
    printf("A simplest database with dict and doubly-linked list.\n");
    printf(" set\t[KEY] [VALUE]\t\t- store Key-Value pair in DB. If key have existed in DB, update Value in DB.\n");
    printf(" get\t[key]\t\t\t- use Key to get Value in DB.\n");
    printf(" del\t[key]\t\t\t- delete key-Value pair in DB.\n");
    printf(" lpush\t[KEY] [VALUE]\t\t- push Value from left to a doubly-linked list in DB.\n");
    printf(" lpop\t[KEY]\t\t\t- pop Value by left from a doubly-linked list in DB.\n");
    printf(" rpush\t[KEY] [VALUE]\t\t- push Value from right to a doubly-linked list in DB.\n");
    printf(" rpop\t[KEY]\t\t\t- pop Value by right from a doubly-linked list in DB.\n");
    printf(" llen\t[KEY]\t\t\t- show length of a doubly-linked list in DB.\n");
    printf(" lrange\t[KEY] [LEFT] [RIGHT]\t- show elements from [LEFT] to [RIGHT] of a doubly-linked list.\n");
    printf(" hset\t[KEY] [FIELD] [VALUE]\n");
    printf(" hget\t[KEY] [FIELD]\n");
    printf(" hdel\t[KEY] [FIELD]\n");
    printf(" hinfo\t[KEY]\n");
    printf(" expire\t[KEY] [TIME]\n");
    printf(" zadd\t[KEY] [MEMBER] [VAL]\n");
    printf(" zcard\t[KEY]\n");
    printf(" zcount\t[KEY] [LEFT] [RIGHT]\n");
    printf(" zinterstore\t[DEST] [SOURCE1] [SOURCE2]\n");
    printf(" zunionstore\t[DEST] [SOURCE1] [SOURCE2]\n");
    printf(" zrange\t[KEY] [LEFT] [RIGHT]\n");
    printf(" zrangebyscore\t[KEY] [LEFT] [RIGHT]\n");
    printf(" zrank\t[KEY] [MEMBER]\n");
    printf(" zrem\t[KEY] [MEMBER]\n");
    printf(" zremrangebyscore\t[KEY] [LEFT] [RIGHT]\n");
    printf(" info \t\t\t\t- list DB state.\n");
    printf(" help \t\t\t\t- list command usage.\n");
    printf(" exit \t\t\t\t- exit and close DB.\n");
}

double _duration(time_t start, time_t end){
    return (double)(end - start) / (CLOCKS_PER_SEC);
}

void hello(){
    printf("******************"
        "*********************\n");
    printf("\tAuthor: chris-pan\n");
    printf("\tVersion: 1.0\n");
    printf("\tA simple Nosql db\n");
    printf("*******************"
        "********************\n");
}

static void stdin_cb(struct ev_loop *loop, ev_io *w, int revents) {
    char buffer[BUFFER_SIZE];
    int argv = 0;
    char args[N_MAX_TOKEN][BUFFER_SIZE];
    ssize_t nread = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    if (nread > 0) {
        buffer[nread] = '\0'; // the last character
        // printf("Received input: %s", buffer);

        if (buffer[nread - 1] == '\n') {
            buffer[nread - 1] = '\0';
        }
        
        if (strncmp(buffer, "exit", 4) == 0) {
            ev_break(EV_A_ EVBREAK_ALL);  // 終止事件循環
            printf("Bye.\n");
            fflush(stdout);
            DB_free(db);
            return;
        }

        char *token = strtok(buffer, " ");
        while (token != NULL) {
            // printf("Token: %s\n", token);
            strcpy(args[argv], token);
            argv++;
            token = strtok(NULL, " ");
        }

        if(strcmp(args[0], "set") == 0){
            if(argv != 3){
                printf("Invalid usage of command `set`.\n");
                help();   
            }else{
                // key = args[1], data = args[2]
                int status = set(db, args[1], args[2]);
                if(status == FAIL)
                    printf("Fatal error, set (%s, %s) fail\n", args[1], args[2]);
                else 
                    printf("Set (%s, %s) successfully.\n", args[1], args[2]);
                // TODO: rehash
            }  
        }
        else if(strcmp(args[0], "get") == 0){
            if(argv != 2){
                printf("Invalid usage of command `get`.\n");
                help();
            }else{
                char *data = get(db, args[1]);
                if(data == NULL)
                    printf("Key `%s` does not exist in DB.\n", args[1]);
                else 
                    printf("%s\n", data);
            }
        }
        else if(strcmp(args[0], "del") == 0){
            if(argv != 2){
                printf("Invalid usage of command `del`.\n");
                help();  
            }else{
                int status = del(db, args[1]);
                if(status == FAIL)
                    printf("Key `%s` does not exist in DB.\n", args[1]);
                else 
                    printf("Delete Key `%s` successfully.\n", args[1]);
            }
        }
        else if(strcmp(args[0], "lpush") == 0){
            if(argv != 3){
                printf("Invalid usage of command `lpush`.\n");
                help();   
            }else{
                // key = args[1], data = args[2]
                int status = lpush(db, args[1], args[2]);
                if(status == FAIL)
                    printf("Fatal error, lpush %s %s fail\n", args[1], args[2]);
                else 
                    printf("lpush %s %s successfully.\n", args[1], args[2]);
            }
        }
        else if(strcmp(args[0], "rpush") == 0){
            if(argv != 3){
                printf("Invalid usage of command `rpush`.\n");
                help(); 
            }else{
                // key = args[1], data = args[2]
                int status = rpush(db, args[1], args[2]);
                if(status == FAIL)
                    printf("Fatal error, rpush %s %s fail\n", args[1], args[2]);
                else 
                    printf("rpush %s %s successfully.\n", args[1], args[2]);
            }
        }
        else if(strcmp(args[0], "lpop") == 0){
            if(argv != 2){
                printf("Invalid usage of command `lpop`.\n");
                help();
            }else{
                // key = args[1]
                int status = lpop(db, args[1]);
                if(status == FAIL)
                    printf("Fatal error, lpop %s fail\n", args[1]);
                else 
                    printf("lpop %s successfully.\n", args[1]);
            }
        }
        else if(strcmp(args[0], "rpop") == 0){
            if(argv != 2){
                printf("Invalid usage of command `rpop`.\n");
                help(); 
            }else{
                // key = args[1]
                int status = rpop(db, args[1]);
                if(status == FAIL)
                    printf("Fatal error, rpop %s fail\n", args[1]);
                else 
                    printf("rpop %s successfully.\n", args[1]);
            }
        }
        else if(strcmp(args[0], "llen") == 0){
            if(argv != 2){
                printf("Invalid usage of command `llen`.\n");
                help(); 
            }else{
                // key = args[1]
                int len = llen(db, args[1]);
                if(len == -1)
                    printf("Fatal error, llen %s fail\n", args[1]);
                else 
                    printf("Length of %s: %d\n", args[1], len);
            }
        }
        else if(strcmp(args[0], "lrange") == 0){
            if(argv != 4){
                printf("Invalid usage of command `lrange`.\n");
                help(); 
            }else{
                // key = args[1]
                int status = lrange(db, args[1], atoi(args[2]), atoi(args[3]));
                if(status == FAIL)
                    printf("Fatal error, lrange %s %s %s fail\n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "hset") == 0){ // hset key field value
            if(argv != 4){
                printf("Invalid usage of command `lrange`.\n");
                help();    
            }else{
                int state = hset(db, args[1], args[2], args[3]);
                if(state == FAIL)
                    printf("Fatal error. Can not set key `%s` as dict or other errors\n", args[1]);
                else 
                    printf("hset %s (%s, %s) successfully.\n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "hget") == 0){ // hget key field
            if(argv != 3){
                printf("Invalid usage of command `lrange`.\n");
                help();    
            }else{
                char *data = hget(db, args[1], args[2]);
                if(data == NULL)
                    printf("key `%s` does not exist in DB or field `%s` does not exist in `%s`\n", args[1], args[2], args[1]);
                else 
                    printf("%s\n", data);
            }
        }
        else if(strcmp(args[0], "hdel") == 0){ // hdel key field
            if(argv != 3){
                printf("Invalid usage of command `lrange`.\n");
                help();    
            }else{
                // TODO:
                int state = hdel(db, args[1], args[2]);
                if(state == FAIL)
                    printf("Fatal error. Can not delete key `%s` by hdel or field `%s` does not exist in dict\n", args[1], args[2]);
                else 
                    printf("hdel %s %s successfully.\n", args[1], args[2]);
            }
        }
        else if(strcmp(args[0], "hinfo") == 0){ // hinfo key
            if(argv != 2){
                printf("Invalid usage of command `hinfo`.\n");
                help();    
            }else{
                // TODO:
                int state = hinfo(db, args[1]);
                if(state == FAIL)
                    printf("Fatal error. Can not get Hashtable info from `%s`\n", args[1]);
            }
        }
        else if(strcmp(args[0], "expire") == 0){
            if(argv != 3){
                printf("Invalid usage of command `expire`.\n");
                help();   
            }else{
                int state = expire(db, args[1], atoi(args[2]));
                if(state == FAIL)
                    printf("Fatal error. Can not set key `%s` expire\n", args[1]);
                else 
                    printf("expire %s %s successfully.\n", args[1], args[2]);
            }
        }
        else if(strcmp(args[0], "zadd") == 0){ // zadd key member val
            if(argv != 4){
                printf("Invalid usage of command `zadd`.\n");
                help();   
            }else{
                // TODO:
                char *endptr;
                errno = 0;  // Reset errno before the conversion
                double val = strtod(args[3], &endptr);

                // Check for various possible errors
                if (endptr == args[3]) {
                    // No digits were found
                    printf("Error: No digits were found\n");
                    goto convertFail;
                } else if (errno == ERANGE) {
                    // The number is out of range for a double
                    printf("Error: Number out of range\n");
                    goto convertFail;
                } else if (*endptr != '\0') {
                    // The string contains extra characters after the number
                    printf("Error: Extra characters after the number: '%s'\n", endptr);
                    goto convertFail;
                } else {
                    // Successful conversion
                    printf("Converted number: %lf\n", val);
                    // goto convertFail;
                    int state = zadd(db, args[1], args[2], val);
                    if(state == FAIL)
                        printf("Fatal error. Can not zadd %s %s %lf\n", args[1], args[2], val);
                    else 
                        printf("zadd %s %s %lf successfully. \n", args[1], args[2], val);
                }
            }
        }
        else if(strcmp(args[0], "zcard") == 0){ // zcard key
            if(argv != 2){
                printf("Invalid usage of command `zcard`.\n");
                help();   
            }else{
                // TODO:
                int len = zcard(db, args[1]);
                if(len == -1)
                    printf("Fatal error. Can not zcard %s %s\n", args[1], args[2]);
                else 
                    printf("%d\n", len);
            }
        }
        else if(strcmp(args[0], "zcount") == 0){ // zcount key left right
            if(argv != 4){
                printf("Invalid usage of command `zcount`.\n");
                help();   
            }else{
                // TODO:
                int len = zcount(db, args[1], atoi(args[2]), atoi(args[3]));
                if(len == -1)
                    printf("Fatal error. Can not zcount %s %s %s \n", args[1], args[2], args[3]);
                else 
                    printf("%d\n", len);
            }
        }
        else if(strcmp(args[0], "zinterstore") == 0){ // zinterstore dest source1 source2
            if(argv != 4){
                printf("Invalid usage of command `zinterstore`.\n");
                help();   
            }else{
                // TODO:
                int state = zinterstore(db, args[1], args[2], args[3]);
                if(state == FAIL)
                    printf("Fatal error. Can not zinterstore %s %s %s \n", args[1], args[2], args[3]);
                else 
                    printf("zinterstore %s %s %s successfully. \n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "zunionstore") == 0){ // zunionstore dest source1 source2
            if(argv != 4){
                printf("Invalid usage of command `zunionstore`.\n");
                help();   
            }else{
                // TODO:
                int state = zunionstore(db, args[1], args[2], args[3]);
                if(state == FAIL)
                    printf("Fatal error. Can not zunionstore %s %s %s \n", args[1], args[2], args[3]);
                else 
                    printf("zunionstore %s %s %s successfully. \n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "zrange") == 0){ // zrange key left right
            if(argv != 4){
                printf("Invalid usage of command `zrange`.\n");
                help();   
            }else{
                // TODO:
                int state = zrange(db, args[1], atoi(args[2]), atoi(args[3]));
                if(state == FAIL)
                    printf("Fatal error. Can not zrange %s %s %s \n", args[1], args[2], args[3]);
                // else 
                //     printf("zrange %s %s %s successfully. \n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "zrangebyscore") == 0){ // zrangebyscore key left right
            if(argv != 4){
                printf("Invalid usage of command `zrangebyscore`.\n");
                help();   
            }else{
                // TODO:
                char *endptr;
                errno = 0;  // Reset errno before the conversion
                double left = strtod(args[2], &endptr);

                // Check for various possible errors
                if (endptr == args[2]) {
                    // No digits were found
                    printf("Error: No digits were found\n");
                    goto convertFail;
                } else if (errno == ERANGE) {
                    // The number is out of range for a double
                    printf("Error: Number out of range\n");
                    goto convertFail;
                } else if (*endptr != '\0') {
                    // The string contains extra characters after the number
                    printf("Error: Extra characters after the number: '%s'\n", endptr);
                    goto convertFail;
                }

                double right = strtod(args[3], &endptr);

                // Check for various possible errors
                if (endptr == args[3]) {
                    // No digits were found
                    printf("Error: No digits were found\n");
                    goto convertFail;
                } else if (errno == ERANGE) {
                    // The number is out of range for a double
                    printf("Error: Number out of range\n");
                    goto convertFail;
                } else if (*endptr != '\0') {
                    // The string contains extra characters after the number
                    printf("Error: Extra characters after the number: '%s'\n", endptr);
                    goto convertFail;
                }

                int state = zrangebyscore(db, args[1], left, right);

                if(state == FAIL)
                    printf("Fatal error. Can not zrangebyscore %s %s %s \n", args[1], args[2], args[3]);
                // else 
                //     printf("zrangebyscore %s %s %s successfully. \n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "zrank") == 0){ // zrank key member
            if(argv != 3){
                printf("Invalid usage of command `zrank`.\n");
                help();   
            }else{
                // TODO:
                int rank = zrank(db, args[1], args[2]);
                if(rank == -1)
                    printf("Fatal error. Can not zrank %s %s \n", args[1], args[2]);
                else 
                    printf("%d\n", rank);
            }
        }
        else if(strcmp(args[0], "zrem") == 0){ // zrem key member
            if(argv != 3){
                printf("Invalid usage of command `zrem`.\n");
                help();   
            }else{
                // TODO:
                int state = zrem(db, args[1], args[2]);
                if(state == FAIL)
                    printf("Fatal error. Can not zrem %s %s \n", args[1], args[2]);
                else 
                    printf("zrem %s %s successfully. \n", args[1], args[2]);
            }
        }
        else if(strcmp(args[0], "zremrangebyscore") == 0){ // zcount key left right
            if(argv != 4){
                printf("Invalid usage of command `zremrangebyscore`.\n");
                help();   
            }else{
                // TODO:
                char *endptr;
                errno = 0;  // Reset errno before the conversion
                double left = strtod(args[2], &endptr);

                // Check for various possible errors
                if (endptr == args[2]) {
                    // No digits were found
                    printf("Error: No digits were found\n");
                    goto convertFail;
                } else if (errno == ERANGE) {
                    // The number is out of range for a double
                    printf("Error: Number out of range\n");
                    goto convertFail;
                } else if (*endptr != '\0') {
                    // The string contains extra characters after the number
                    printf("Error: Extra characters after the number: '%s'\n", endptr);
                    goto convertFail;
                }

                double right = strtod(args[3], &endptr);

                // Check for various possible errors
                if (endptr == args[3]) {
                    // No digits were found
                    printf("Error: No digits were found\n");
                    goto convertFail;
                } else if (errno == ERANGE) {
                    // The number is out of range for a double
                    printf("Error: Number out of range\n");
                    goto convertFail;
                } else if (*endptr != '\0') {
                    // The string contains extra characters after the number
                    printf("Error: Extra characters after the number: '%s'\n", endptr);
                    goto convertFail;
                }

                int state = zremrangebyscore(db, args[1], left, right);
                if(state == FAIL)
                    printf("Fatal error. Can not zremrangebyscore %s %s %s \n", args[1], args[2], args[3]);
                else 
                    printf("zremrangebyscore %s %s %s successfully. \n", args[1], args[2], args[3]);
            }
        }
        else if(strcmp(args[0], "info") == 0){
            if(argv != 1){
                printf("Invalid usage of command `info`.\n");
                help();   
            }else{
                printf("Database state: \n");
                dict_info(db->dict);
            }
        }
        else if(strcmp(args[0], "exit") == 0){
            if(argv != 1){
                printf("Invalid usage of command `exit`.\n");
                help();   
            }else{
                printf("Bye.\n");
            }
        }
        else if(strcmp(args[0], "help") == 0){
            if(argv != 1){
                printf("Invalid usage of command `help`.\n"); // ???
            }
            help();
        }
        else{
            printf("Invalid command.\n");
            help();
        }
    }
    convertFail:
        // do nothing
    printf("db > ");  // db > 提示符
    fflush(stdout);
}

int main(){
    
    db = DB_create(BUCKET_SIZE_LIST[0]);

    hello();
    printf("db > ");  // db > 提示符
    fflush(stdout);

    initialize_loop();
    ev_io stdin_watcher;

    ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    // ev_idle_start(loop, &(db->dict->watcher->w)); // In the beginning, only this watcher need to be manually start

    ev_run(loop, 0);
}