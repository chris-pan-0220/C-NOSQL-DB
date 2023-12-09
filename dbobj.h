#ifndef DBOBJ_H
#define DBOBJ_H

typedef struct DBobj{
    unsigned type;
    void* value; // point to low-level data type, ex: dlist, set, tree...
} DBobj;

DBobj *DBobj_create(unsigned type);// create empty node | API
DBobj *DBobj_create_list(DBobj *dbobj);
DBobj *DBobj_create_string(DBobj *dbobj);
void* DBobj_get_val(DBobj *dbobj);
// int DBobj_set_string(DBobj *dbobj, const char * const val); // it's not needed
// DBobj *DBobj_create_set(DBobj *dbobj);  // TODO
// DBobj *DBobj_create_dict(DBobj *dbobj); // TODO
int DBobj_free(DBobj *dbobj);
int DBobj_free_string(DBobj *dbobj);
int DBobj_free_list(DBobj *dbobj);

#endif