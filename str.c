#include <stdlib.h>
#include <string.h>
#include "status.h"
#include "str.h"

// create a empty String
String *string_create(){
    String *str = (String*)malloc(sizeof(String));
    str->value = NULL;
    str->len = 0;
    return str;
}

// set String with value
int string_set(String *dest, const char * const source){
    if(source == NULL) // empty string is valid
        return FAIL;

    size_t len = strlen(source);

    if(len > dest->len){
        free(dest->value);
        dest->value = (char*)malloc(sizeof(char)*(len + 1));
    }
    strncpy(dest->value, source, len+1);
    dest->len = len;

    return SUCCESS;
}

// free string
int string_free(String *str){
    if(str == NULL)
        return SUCCESS;
    free(str->value);
    free(str);
    return SUCCESS;
}