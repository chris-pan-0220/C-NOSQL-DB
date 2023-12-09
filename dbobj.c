#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dbobj.h"
#include "datatype.h"
#include "status.h"
#include "dlist.h"
#include "str.h"

DBobj *DBobj_create(unsigned type){
    DBobj *dbobj = (DBobj*)malloc(sizeof(DBobj));
    dbobj->type = type;
    switch (type){
        case TYPE_STRING:
            return DBobj_create_string(dbobj);
        case TYPE_LIST:
            return DBobj_create_list(dbobj);
        default:
            return NULL; 
    }
}

DBobj *DBobj_create_list(DBobj *dbobj){ // problem: turn to static function
    dbobj->value = dlist_create();
    return dbobj;
}

// int DBobj_set_string(DBobj *dbobj, const char * const val){ // why not DBobj ??? 
//     if(dbobj == NULL || val == NULL || dbobj->type != TYPE_STRING) 
//         return FAIL;
//     size_t l1 = strlen((char*)(dbobj->value));
//     size_t l2 = strlen(val);
//     if(l1 < l2){
//         free((char*)(dbobj->value));
//         dbobj->value = (char*)malloc(sizeof(char)*(l2+1));
//     }
//     strncpy((char*)(dbobj->value), val, l2);
//     return SUCCESS;
// }

DBobj *DBobj_create_string(DBobj *dbobj){
    dbobj->value = string_create();
    return dbobj;
}

void* DBobj_get_val(DBobj *dbobj){
    if(dbobj == NULL) 
        return NULL;
    return dbobj->value;
}

/* 
    Everything is DBobj, also data in adlist, dict, etc.
    if embedded struct, we have to free DBobj recursively
*/ 

int DBobj_free(DBobj *dbobj){
    int state = SUCCESS;
    switch (dbobj->type){
        case TYPE_STRING:
            state = DBobj_free_string(dbobj);
            break;
        case TYPE_LIST:
            state = DBobj_free_list(dbobj);
            break;
        case TYPE_DICT:
            return FAIL;  // TODO
        case TYPE_SET:
            return FAIL;  // TODO
        default:
            return FAIL; 
    }
    assert(state == SUCCESS);
    free(dbobj);
    return state;
}

int DBobj_free_string(DBobj *dbobj){ // turn to static function !
    if(dbobj->type != TYPE_STRING)
        return FAIL;
    return string_free(dbobj->value);
}

int DBobj_free_list(DBobj *dbobj){ // turn to static function !
    if(dbobj->type != TYPE_LIST)
        return FAIL;
    return dlist_free(dbobj->value); // state
}

// int DBObj_free_set(){ // TODO:

// }

// int DBObj_free_dict(){ // TODO:

// }

// DBobj *DBobj_create_set() // TODO:

// DBobj *DBobj_create_dict() // TODO: