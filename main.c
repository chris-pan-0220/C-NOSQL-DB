#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>

#include "db.h"
#include "dict.h"
#include "status.h"

#define BUFFER_SIZE 256 
#define N_MAX_TOKEN 16
#define SAVE_DURATION 5 // 1000ms
#define STORAGE_BASE ".\\storage"
#define STORAGE ".\\storage\\db.txt"

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

int main(){
    DB *db = DB_create(BUCKET_SIZE_LIST[0]);
    
    printf("******************"
        "*********************\n");
    printf("\tAuthor: chris-pan\n");
    printf("\tVersion: 1.0\n");
    printf("\tA simple Nosql db\n");
    printf("*******************"
        "********************\n");

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

    while (1){
        char command[BUFFER_SIZE];
        int argv = 0;
        char args[N_MAX_TOKEN][BUFFER_SIZE];
        printf("DataBase > ");
        if (fgets(command, sizeof(command), stdin) == NULL) 
            break;
        command[strcspn(command, "\n")] = '\0';

        // get tokens from command
        char *token = strtok(command, " ");
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
                continue;
            }
            // key = args[1], data = args[2]
            int status = set(db, args[1], args[2]);
            if(status == FAIL)
                printf("Fatal error, set (%s, %s) fail\n", args[1], args[2]);
            else 
                printf("Set (%s, %s) successfully.\n", args[1], args[2]);

            // TODO: rehash
        }
        else if(strcmp(args[0], "get") == 0){
            if(argv != 2){
                printf("Invalid usage of command `get`.\n");
                help();  
                continue;  
            }
            char *data = get(db, args[1]);
            if(data == NULL)
                printf("Key `%s` does not exist in DB.\n", args[1]);
            else 
                printf("%s\n", data);
        }
        else if(strcmp(args[0], "del") == 0){
            if(argv != 2){
                printf("Invalid usage of command `del`.\n");
                help();   
                continue; 
            }
            int status = del(db, args[1]);
            if(status == FAIL)
                printf("Key `%s` does not exist in DB.\n", args[1]);
            else 
                printf("Delete Key `%s` successfully.\n", args[1]);
        }
        else if(strcmp(args[0], "lpush") == 0){
            if(argv != 3){
                printf("Invalid usage of command `lpush`.\n");
                help();    
                continue;
            }
            // key = args[1], data = args[2]
            int status = lpush(db, args[1], args[2]);
            if(status == FAIL)
                printf("Fatal error, lpush %s %s fail\n", args[1], args[2]);
            else 
                printf("lpush %s %s successfully.\n", args[1], args[2]);
        }
        else if(strcmp(args[0], "rpush") == 0){
            if(argv != 3){
                printf("Invalid usage of command `rpush`.\n");
                help();    
                continue;
            }
            // key = args[1], data = args[2]
            int status = rpush(db, args[1], args[2]);
            if(status == FAIL)
                printf("Fatal error, rpush %s %s fail\n", args[1], args[2]);
            else 
                printf("rpush %s %s successfully.\n", args[1], args[2]);
        }
        else if(strcmp(args[0], "lpop") == 0){
            if(argv != 2){
                printf("Invalid usage of command `lpop`.\n");
                help();    
                continue;
            }
            // key = args[1]
            int status = lpop(db, args[1]);
            if(status == FAIL)
                printf("Fatal error, lpop %s fail\n", args[1]);
            else 
                printf("lpop %s successfully.\n", args[1]);
        }
        else if(strcmp(args[0], "rpop") == 0){
            if(argv != 2){
                printf("Invalid usage of command `rpop`.\n");
                help();    
                continue;
            }
            // key = args[1]
            int status = rpop(db, args[1]);
            if(status == FAIL)
                printf("Fatal error, rpop %s fail\n", args[1]);
            else 
                printf("rpop %s successfully.\n", args[1]);
        }
        else if(strcmp(args[0], "llen") == 0){
            if(argv != 2){
                printf("Invalid usage of command `llen`.\n");
                help();    
                continue;
            }
            // key = args[1]
            int len = llen(db, args[1]);
            if(len == -1)
                printf("Fatal error, llen %s fail\n", args[1]);
            else 
                printf("Length of %s: %d\n", args[1], len);
        }
        else if(strcmp(args[0], "lrange") == 0){
            if(argv != 4){
                printf("Invalid usage of command `lrange`.\n");
                help();    
                continue;
            }
            // key = args[1]
            
            int status = lrange(db, args[1], atoi(args[2]), atoi(args[3]));
            if(status == FAIL)
                printf("Fatal error, lrange %s %s %s fail\n", args[1], args[2], args[3]);
        }
        // else if(strcmp(args[0], "info") == 0){
        //     if(argv != 1){
        //         printf("Invalid usage of command `info`.\n");
        //         help();   
        //         continue; 
        //     } 
        //     hashtable_info(hashTable);
        // }
        else if(strcmp(args[0], "exit") == 0){
            if(argv != 1){
                printf("Invalid usage of command `exit`.\n");
                help();   
                continue; 
            }
            printf("Bye.\n");
            break;
        }
        else if(strcmp(args[0], "help") == 0){
            if(argv != 1){
                printf("Invalid usage of command `help`.\n");
                help();   
                continue; 
            }
            help();
        }
        else{
            printf("Invalid command.\n");
            help();
        }

        // argv = 0;
        // todo: reset command, argv, argc
    }
    

    DB_free(db);

}