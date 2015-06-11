#include <stdlib.h>
#include "cache_list.h"

struct Cache_List *Cache_List_Create(){
    struct Cache_List* list=(struct Cache_List*)malloc(sizeof(struct Cache_List));
    list->next=list;
    list->pheader=NULL;
    list->prev=list;
    return list;
}
/*
 * insertion à la fin
 */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* nouv = (struct Cache_List*)malloc(sizeof(struct Cache_List));
    nouv->pheader = pbh;
    nouv->prev = list->prev;
    nouv->next = list;

    list->prev->next = nouv;
    list->prev = nouv;
}

void Cache_List_Clear(struct Cache_List *list) {
    for(struct Cache_List *i = list->prev; i->prev != list ; i = i->prev  ){
        free(i->pheader);
        free(i);
    }

    free(list->pheader);
    list->pheader = NULL;
}

void Cache_List_Delete(struct Cache_List *list){
    Cache_List_Clear(list);
    free(list);
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* nouv=(struct Cache_List*)malloc(sizeof(struct Cache_List));

    nouv->pheader=list->pheader;
    list->pheader = pbh;

    nouv->next=list->next;
    nouv->prev=list;

    list->next=nouv;
}
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List *cl = NULL;
    for( cl = list; cl->pheader != pbh && cl != list->prev; cl = cl->next  );

    if( cl->pheader != pbh ){
        Cache_List_Append(list, pbh);
        return;
    }

    cl->prev->next = cl->next;
    cl->next->prev = cl->prev;

    cl->next = list;
    cl->prev = list->prev;
    list->prev->next = cl;
    list->prev = cl;

}

void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List *cl = NULL;
    for( cl = list; cl->pheader != pbh && cl != list->prev; cl = cl->next  );

    if( cl->pheader != pbh ){
        Cache_List_Append(list, pbh);
        return;
    }

    cl->prev->next = cl->next;
    cl->next->prev = cl->prev;

    //TODO

}

struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
    struct Cache_Block_Header* res=list->pheader;
    struct Cache_List* tmp=list->prev;
    list=list->next;
    list->prev=tmp;
    return res;
}

struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
    struct Cache_Block_Header* res;
    res=(list->prev)->pheader;
    return res;
}