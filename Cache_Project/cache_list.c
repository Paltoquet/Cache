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
    printf("Append \n");
    struct Cache_List* nouv = (struct Cache_List*)malloc(sizeof(struct Cache_List));
    nouv->pheader = pbh;
    nouv->prev = list->prev;
    nouv->next = list;

    list->prev->next = nouv;
    list->prev = nouv;

}

void Cache_List_Clear(struct Cache_List *list) {
    printf("clear\n");

    for(struct Cache_List *i = list->prev; i != list ; i = i->prev  ){
        free(i);
    }

    list->next = list;
    list->prev = list;
    list->pheader = NULL;
}

void Cache_List_Delete(struct Cache_List *list){
    printf("delete\n");
    Cache_List_Clear(list);
    free(list);
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
    printf("Prepend\n");
    struct Cache_List* nouv=(struct Cache_List*)malloc(sizeof(struct Cache_List));

    nouv->pheader=list->pheader;
    list->pheader = pbh;

    nouv->next=list->next;
    nouv->prev=list;

    list->next=nouv;
}
void Cache_List_Move_To_End(struct Cache_List *list, struct Cache_Block_Header *pbh){
    printf("move to end %p \n", list->prev);

    struct Cache_List *cl = NULL;
    //printf("1..-2 %p %p %p\n", list->pheader, list->next,  pbh);
    for( cl = list; cl->pheader != pbh && cl != list->prev; cl = cl->next  ){
        //printf("%p %p \n" ,cl->pheader, cl->next->pheader);
       // printf("coucou\n");
        //printf("%p\n", cl->next->next);
    }


    printf("1..-1\n");
    if( cl->pheader != pbh ){
        printf("1..\n");
        Cache_List_Append(list, pbh);
        return;
    }

    printf("2\n");
    cl->prev->next = cl->next;
    cl->next->prev = cl->prev;

    printf("3\n");
    cl->next = list;
    cl->prev = list->prev;
    list->prev->next = cl;
    list->prev = cl;

}

void Cache_List_Move_To_Begin(struct Cache_List *list, struct Cache_Block_Header *pbh){
    printf("move to begin\n");
    struct Cache_List *cl = NULL;
    for( cl = list; cl->pheader != pbh && cl != list->prev; cl = cl->next  );

    if( cl->pheader != pbh ){
        Cache_List_Append(list, pbh);
        return;
    }

    cl->prev->next = cl->next;
    cl->next->prev = cl->prev;

    //On ajoute en 2 eme position puis on modifie le premier
    list->next->prev=cl;
    cl->next=list->next;
    cl->prev=list;
    cl->pheader=list->pheader;
    list->pheader=pbh;
    list->next=cl;
}

struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
    printf("remove first\n");

    for( struct Cache_List *cl = list; cl->next != list; cl = cl->next  ){
        cl->pheader = cl->next->pheader;
    }

    return Cache_List_Remove_Last(list);
}

struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
    printf("remove last\n");
    struct Cache_Block_Header* res=(list->prev)->pheader;
    list->prev->prev->next=list;
    list->prev=list->prev->prev;

    return res;
}

bool Cache_List_Is_Empty(struct Cache_List *list){
    printf("is empty\n");
    if(list->pheader == NULL){
        return true;
    }
    else return false;
}

struct Cache_Block_Header *Cache_List_Remove(struct Cache_List *list,
                                             struct Cache_Block_Header *pbh){
    printf("remove\n");
    if( Cache_List_Is_Empty(list) ){
        return NULL;
    }

    struct Cache_List *cl = NULL;
    for( cl = list; cl->pheader != pbh && cl != list->prev; cl = cl->next  );

    if(  cl->pheader != pbh ){
        return NULL;
    }

    cl->prev->next = cl->next;
    cl->next->prev = cl->prev;

    free(cl);
    return pbh;
}

