#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "status.h"
#include "sortedSet.h"

SortList *sortList_create(){
    SortList *list = (SortList *)malloc(sizeof(SortList));
    list->len = 0;
    list->head = NULL;
    return list;
}

int sortList_insert(SortList *list, const char * const key, double val){
    if(list == NULL || key == NULL)
        return FAIL;
    
    SortListNode *prev = NULL;
    SortListNode *current = list->head;
    while(current != NULL){
        if(current->score > val){
            break;
        }
        prev = current;
        current = current->next;
    }

    SortListNode *node = (SortListNode *)malloc(sizeof(SortListNode));
    
    node->score = val;

    size_t len = strlen(key);
    node->key = (char*)malloc(sizeof(char)*(len+1));
    strcpy(node->key, key);

    if(prev == NULL){
        list->head = node;
        node->next = current;
    }else{
        prev->next = node;
        node->next = current;
    }

    list->len++;

    return SUCCESS;
}

SortList *sortList_inter(SortList *source1, SortList *source2){
    if(source1 == NULL || source2 == NULL)
        return NULL;

    SortListNode *ptr1 = source1->head, *ptr2 = source2->head;
    SortList *dest = sortList_create();
    
    while(ptr1 != NULL){
        ptr2 = source2->head;
        int found = 0;
        while(ptr2 != NULL){
            if(strcmp(ptr1->key, ptr2->key) == 0){
                found = 1;
                break;
            }
            ptr2 = ptr2->next;
        }
        if(found){// s1 & s2
            int state = sortList_insert(dest, ptr1->key, ptr1->score + ptr2->score);
            // TODO: test state
        }
        ptr1 = ptr1->next;
    }

    return dest;
}

SortList *sortList_union(SortList *source1, SortList *source2){
    if(source1 == NULL || source2 == NULL)
        return NULL;

    SortListNode *ptr1 = source1->head, *ptr2 = source2->head;
    SortList *dest = sortList_create();

    while(ptr1 != NULL){
        ptr2 = source2->head;
        int found = 0;
        while(ptr2 != NULL){
            if(strcmp(ptr1->key, ptr2->key) == 0){
                found = 1;
                break;
            }
            ptr2 = ptr2->next;
        }
        if(found){ // s1 & s2
            int state = sortList_insert(dest, ptr1->key, ptr1->score + ptr2->score);
            // TODO: test state
        }else{ // s1 - s2
            int state = sortList_insert(dest, ptr1->key, ptr1->score);
        }
        ptr1 = ptr1->next;
    }

    ptr1 = source1->head, ptr2 = source2->head;

    while(ptr2 != NULL){
        ptr1 = source1->head;
        int notFound = 1;
        while(ptr1 != NULL){
            if(strcmp(ptr1->key, ptr2->key) == 0){
                notFound = 0;
                break;
            }
            ptr1 = ptr1->next;
        }
        if(notFound){ // s2 - s1
            int state = sortList_insert(dest, ptr2->key, ptr2->score);
            // TODO: test state
        }
        ptr2 = ptr2->next;
    }

    return dest;
}

int sortList_count_byscore(SortList *list, double left, double right){
    if(list == NULL || left > right)
        return -1;

    SortListNode *ptr = list->head;

    while(ptr != NULL && ptr->score < left)
        ptr = ptr->next;
    
    int cnt = 0;
    while(ptr != NULL && ptr->score <= right){
        ptr = ptr->next;
        cnt++;
    }

    return cnt;
}

int sortList_range_byindex(SortList *list, int left, int right){
    if(list == NULL)
        return FAIL;
    
    if(left < 0)
        left = list->len + left; // ex: 10 + (-1), the last element
    if(right < 0)
        right = list->len + right; // ex: 10 + (-1), the last element
    if(left < 0)
        left = 0;
    if(right >= list->len)
        right = list->len - 1;
    if(left > right)
        return FAIL;

    SortListNode *ptr = list->head;

    int idx = 0;
    while(ptr != NULL && idx < left){
        ptr = ptr->next;
        idx++;
    }// idx = left here

    while(ptr != NULL && idx <= right){
        printf("%d) %s: %lf\n", idx - left + 1, ptr->key, ptr->score);// ptr->value is DBobj
        ptr = ptr->next;
        idx++;
    }

    return SUCCESS;
}

int sortList_range_byscore(SortList *list, double left, double right){
    if(list == NULL || left > right)
        return FAIL;

    SortListNode *ptr = list->head;

    while(ptr != NULL && ptr->score < left)
        ptr = ptr->next;
    
    int idx = 1;
    while(ptr != NULL && ptr->score <= right){
        printf("%d) %s: %lf\n", idx, ptr->key, ptr->score);// ptr->value is DBobj
        ptr = ptr->next;
        idx++;
    }

    return SUCCESS;
}

int sortList_rank(SortList *list, const char * const key){
    if(list == NULL|| key == NULL)
        return FAIL;
    
    int rank = 0, found = 0;
    SortListNode *ptr = list->head;
    while(ptr != NULL){
        if(strcmp(ptr->key, key) == 0){
            found = 1;
            break;
        }
        ptr = ptr->next;
        rank++;
    }

    if(found){
        return rank;
    }else{
        return -1;
    }
}

int sortList_delete_bykey(SortList *list, const char * const key){
    if(list == NULL || key == NULL)
        return FAIL;

    SortListNode *prev = NULL, *current = list->head;
    int found = 0;
    while(current != NULL){
        if(strcmp(current->key, key) == 0){
            found = 1;
            break;
        }
        prev = current;
        current = current->next;
    }

    if(found){
        if(prev == NULL){
            list->head = current->next;
            free(current->key);
            free(current);
        }else{
            prev->next = current->next;
            free(current->key);
            free(current);
        }

        list->len--;

        return SUCCESS;
    }else{
        return FAIL;
    }
}

int sortList_delete_byscore(SortList *list, double left, double right){
    if(list == NULL)
        return FAIL;

    SortListNode *prev = NULL, *current = list->head;

    while(current != NULL && current->score < left){// TODO: check safe ?
        prev = current;
        current = current->next;
    }
    
    while(current != NULL && current->score <= right){// TODO: check safe ?
        if(prev == NULL){
            list->head = current->next;
            free(current->key);
            free(current);
            // point to next node
            current = list->head;
        }else{
            prev->next = current->next;
            // free current
            free(current->key);
            free(current);
            // point to next node
            current = prev->next;
        }
        
        list->len--;
    }

    return SUCCESS;
}

int sortList_free(SortList *list){
    if(list == NULL)
        return FAIL;
    
    SortListNode *current = list->head;
    while(current != NULL){
        list->head = current->next;
        free(current->key);
        free(current);
        current = list->head;
    }
    free(list);

    return SUCCESS;
}