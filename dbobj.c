#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dbobj.h"
#include "datatype.h"
#include "status.h"
#include "dlist.h"
#include "str.h"
#include "dict.h"
#include "sortedSet.h"

DBobj *DBobj_create(unsigned type){
    DBobj *dbobj = (DBobj*)malloc(sizeof(DBobj));
    dbobj->type = type;
    switch (type){
        case TYPE_STRING:
            return DBobj_create_string(dbobj);
        case TYPE_LIST:
            return DBobj_create_list(dbobj);
        case TYPE_DICT:
            return DBobj_create_dict(dbobj);
        case TYPE_SET:
            return DBobj_create_set(dbobj);
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

DBobj *DBobj_create_dict(DBobj *dbobj){
    dbobj->value = dict_create(BUCKET_SIZE_LIST[0]); // TODO: default size
    return dbobj;
}

DBobj *DBobj_create_set(DBobj *dbobj){
    dbobj->value = sortList_create(); // TODO: default size
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

int DBobj_free(DBobj *dbobj){ // TODO: return `state`
    int state = SUCCESS;
    switch (dbobj->type){
        case TYPE_STRING:
            state = DBobj_free_string(dbobj);
            break;
        case TYPE_LIST:
            state = DBobj_free_list(dbobj);
            break;
        case TYPE_DICT:
            state = DBobj_free_dict(dbobj);
            break;
        case TYPE_SET:
            state = DBobj_free_set(dbobj);
            break;
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

int DBobj_free_dict(DBobj *dbobj){
    if(dbobj->type != TYPE_DICT)
        return FAIL;
    return dict_free(dbobj->value);
}

int DBobj_free_set(DBobj *dbobj){
    if(dbobj->type != TYPE_SET)
        return FAIL;
    return sortList_free(dbobj->value);
}


// DBobj *DBobj_create_dict() // TODO: