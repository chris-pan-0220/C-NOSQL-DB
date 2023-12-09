#ifndef STR_H 
#define STR_H

typedef struct String{
    char *value;
    size_t len; // length of string
} String;

String *string_create();
int string_set(String *dest, const char * const source);
int string_free(String *str);

#endif