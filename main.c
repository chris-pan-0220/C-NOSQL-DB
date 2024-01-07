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

#define BUFFER_SIZE 256 
#define N_MAX_TOKEN 16
#define SAVE_DURATION 5 // 1000ms
#define STORAGE_BASE ".\\storage"
#define STORAGE ".\\storage\\db.txt"

DB *db;

void help(){
    printf("Usage: \n");
    printf("A simplest database with dict and doubly-linked list.\n");
    printf(" set\t[KEY] [VALUE]\t\t- store Key-Value pair in DB. If key have existed in DB, update Value in DB.\n");
    printf(" get\t[key]\t\t\t- use Key to get Value in DB.\n");
    printf(" del\t[key]\t\t\t- delete key-Value pair in DB.\n");
    // printf(" info \t\t\t- list DB information.\n");
    // printf(" keys * \t\t\t- list all Key-Value pairs in DB.\n");
    printf(" lpush\t[KEY] [VALUE]\t\t- push Value from left to a doubly-linked list in DB.\n");
    printf(" lpop\t[KEY]\t\t\t- pop Value by left from a doubly-linked list in DB.\n");
    printf(" rpush\t[KEY] [VALUE]\t\t- push Value from right to a doubly-linked list in DB.\n");
    printf(" rpop\t[KEY]\t\t\t- pop Value by right from a doubly-linked list in DB.\n");
    printf(" llen\t[KEY]\t\t\t- show length of a doubly-linked list in DB.\n");
    printf(" lrange\t[KEY] [LEFT] [RIGHT]\t- show elements from [LEFT] to [RIGHT] of a doubly-linked list.\n");
    printf(" hset\t[KEY] [FIELD] [VALUE]\n");
    printf(" hget\t[KEY] [FIELD]\n");
    printf(" hdel\t[KEY] [FIELD]\n");
    printf(" expire\t[KEY] [TIME]\n");
    printf(" help \t\t\t\t- list command usage.\n");
    printf(" exit \t\t\t\t- exit and close DB.\n");
}

// void read(){

// }

// void write(){

// }

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
        else if(strcmp(args[0], "expire") == 0){
            if(argv != 3){
                printf("Invalid usage of command `expire`.\n");
                help();   
            }else{
                // char *endptr;
                // uint64_t time = strtol(args[2], &endptr, 10);

                // check error
                // if ((errno == ERANGE && (value == UINT64_MAX || value == LONG_MIN)) || (errno != 0 && time == 0)) {
                //     perror("strtol");
                //     return 1;
                // }

                // if (endptr == input) {
                //     printf("没有找到数字\n");
                //     return 1;
                // }

                // if (*endptr != '\n' && *endptr != '\0') {  // 检查是否有额外的字符
                //     printf("额外的字符： %s\n", endptr);
                //     return 1;
                // }

                int state = expire(db, args[1], atoi(args[2]));
                if(state == FAIL)
                    printf("Fatal error. Can not set key `%s` expire\n", args[1]);
                else 
                    printf("expire %s %s successfully.\n", args[1], args[2]);
            }
        }
        // else if(strcmp(args[0], "info") == 0){
        //     if(argv != 1){
        //         printf("Invalid usage of command `info`.\n");
        //         help();   
        //          
        //     } 
        //     hashtable_info(hashTable);
        // }
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
    printf("db > ");  // db > 提示符
    fflush(stdout);
}

int main(){
    
    db = DB_create(BUCKET_SIZE_LIST[0]);
    hello();
    printf("db > ");  // db > 提示符
    fflush(stdout);

    struct ev_loop *loop = initialize_loop();
    ev_io stdin_watcher;

    ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    ev_run(loop, 0);

    // init storage
    // create storage dir if 
    // const char* file;
    // file = STORAGE_BASE;
    // struct stat sb;

    // if(!(stat(file, &sb) == 0 && S_ISDIR(sb.st_mode))){ // get file state and store to sb, check sb.st_mode
    //     mkdir(STORAGE_BASE);
    //     printf("Create storage folder in %s\n", STORAGE_BASE);
    //     // todo: check error
    // }
    
    // FILE * fp;
    // fp = fopen(STORAGE, "a+");
    // read DB 
    // fgets(command, sizeof(command), fp);
    // fprintf(fp, "We are in 2024\n");
    // printf("data read from DB is: %s\n", command);
    // fclose(fp);

    // while (1){
    //     char command[BUFFER_SIZE];
    //     int argv = 0;
    //     char args[N_MAX_TOKEN][BUFFER_SIZE];
    //     printf("DataBase > ");
    //     if (fgets(command, sizeof(command), stdin) == NULL) 
    //         break;
    //     command[strcspn(command, "\n")] = '\0';

    //     // get tokens from command
    //     char *token = strtok(command, " ");
    //     while (token != NULL) {
    //         // printf("Token: %s\n", token);
    //         strcpy(args[argv], token);
    //         argv++;
    //         token = strtok(NULL, " ");
    //     }
        
    //     if(strcmp(args[0], "set") == 0){
    //         if(argv != 3){
    //             printf("Invalid usage of command `set`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1], data = args[2]
    //         int status = set(db, args[1], args[2]);
    //         if(status == FAIL)
    //             printf("Fatal error, set (%s, %s) fail\n", args[1], args[2]);
    //         else 
    //             printf("Set (%s, %s) successfully.\n", args[1], args[2]);

    //         // TODO: rehash
    //     }
    //     else if(strcmp(args[0], "get") == 0){
    //         if(argv != 2){
    //             printf("Invalid usage of command `get`.\n");
    //             help();  
    //             continue;  
    //         }
    //         char *data = get(db, args[1]);
    //         if(data == NULL)
    //             printf("Key `%s` does not exist in DB.\n", args[1]);
    //         else 
    //             printf("%s\n", data);
    //     }
    //     else if(strcmp(args[0], "del") == 0){
    //         if(argv != 2){
    //             printf("Invalid usage of command `del`.\n");
    //             help();   
    //             continue; 
    //         }
    //         int status = del(db, args[1]);
    //         if(status == FAIL)
    //             printf("Key `%s` does not exist in DB.\n", args[1]);
    //         else 
    //             printf("Delete Key `%s` successfully.\n", args[1]);
    //     }
    //     else if(strcmp(args[0], "lpush") == 0){
    //         if(argv != 3){
    //             printf("Invalid usage of command `lpush`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1], data = args[2]
    //         int status = lpush(db, args[1], args[2]);
    //         if(status == FAIL)
    //             printf("Fatal error, lpush %s %s fail\n", args[1], args[2]);
    //         else 
    //             printf("lpush %s %s successfully.\n", args[1], args[2]);
    //     }
    //     else if(strcmp(args[0], "rpush") == 0){
    //         if(argv != 3){
    //             printf("Invalid usage of command `rpush`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1], data = args[2]
    //         int status = rpush(db, args[1], args[2]);
    //         if(status == FAIL)
    //             printf("Fatal error, rpush %s %s fail\n", args[1], args[2]);
    //         else 
    //             printf("rpush %s %s successfully.\n", args[1], args[2]);
    //     }
    //     else if(strcmp(args[0], "lpop") == 0){
    //         if(argv != 2){
    //             printf("Invalid usage of command `lpop`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1]
    //         int status = lpop(db, args[1]);
    //         if(status == FAIL)
    //             printf("Fatal error, lpop %s fail\n", args[1]);
    //         else 
    //             printf("lpop %s successfully.\n", args[1]);
    //     }
    //     else if(strcmp(args[0], "rpop") == 0){
    //         if(argv != 2){
    //             printf("Invalid usage of command `rpop`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1]
    //         int status = rpop(db, args[1]);
    //         if(status == FAIL)
    //             printf("Fatal error, rpop %s fail\n", args[1]);
    //         else 
    //             printf("rpop %s successfully.\n", args[1]);
    //     }
    //     else if(strcmp(args[0], "llen") == 0){
    //         if(argv != 2){
    //             printf("Invalid usage of command `llen`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1]
    //         int len = llen(db, args[1]);
    //         if(len == -1)
    //             printf("Fatal error, llen %s fail\n", args[1]);
    //         else 
    //             printf("Length of %s: %d\n", args[1], len);
    //     }
    //     else if(strcmp(args[0], "lrange") == 0){
    //         if(argv != 4){
    //             printf("Invalid usage of command `lrange`.\n");
    //             help();    
    //             continue;
    //         }
    //         // key = args[1]
    //         int status = lrange(db, args[1], atoi(args[2]), atoi(args[3]));
    //         if(status == FAIL)
    //             printf("Fatal error, lrange %s %s %s fail\n", args[1], args[2], args[3]);
    //     }
    //     else if(strcmp(args[0], "hset") == 0){ // hset key field value
    //         if(argv != 4){
    //             printf("Invalid usage of command `lrange`.\n");
    //             help();    
    //             continue;
    //         }
    //         int state = hset(db, args[1], args[2], args[3]);
    //         if(state == FAIL)
    //             printf("Fatal error. Can not set key `%s` as dict or other errors\n", args[1]);
    //         else 
    //             printf("hset %s (%s, %s) successfully.\n", args[1], args[2], args[3]);
    //     }
    //     else if(strcmp(args[0], "hget") == 0){ // hget key field
    //         if(argv != 3){
    //             printf("Invalid usage of command `lrange`.\n");
    //             help();    
    //             continue;
    //         }
    //         char *data = hget(db, args[1], args[2]);
    //         if(data == NULL)
    //             printf("key `%s` does not exist in DB or field `%s` does not exist in `%s`\n", args[1], args[2], args[1]);
    //         else 
    //             printf("%s\n", data);
    //     }
    //     else if(strcmp(args[0], "hdel") == 0){ // hdel key field
    //         if(argv != 3){
    //             printf("Invalid usage of command `lrange`.\n");
    //             help();    
    //             continue;
    //         }
    //         // TODO:
    //         int state = hdel(db, args[1], args[2]);
    //         if(state == FAIL)
    //             printf("Fatal error. Can not delete key `%s` by hdel or field `%s` does not exist in dict\n", args[1], args[2]);
    //         else 
    //             printf("hdel %s %s successfully.\n", args[1], args[2]);
    //     }
    //     // else if(strcmp(args[0], "info") == 0){
    //     //     if(argv != 1){
    //     //         printf("Invalid usage of command `info`.\n");
    //     //         help();   
    //     //         continue; 
    //     //     } 
    //     //     hashtable_info(hashTable);
    //     // }
    //     else if(strcmp(args[0], "exit") == 0){
    //         if(argv != 1){
    //             printf("Invalid usage of command `exit`.\n");
    //             help();   
    //             continue; 
    //         }
    //         printf("Bye.\n");
    //         break;
    //     }
    //     else if(strcmp(args[0], "help") == 0){
    //         if(argv != 1){
    //             printf("Invalid usage of command `help`.\n");
    //             help();   
    //             continue; 
    //         }
    //         help();
    //     }
    //     else{
    //         printf("Invalid command.\n");
    //         help();
    //     }

    //     // argv = 0;
    //     // todo: reset command, argv, argc
    // }
    

    // DB_free(db);

}