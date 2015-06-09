#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h"




//Renvoie la structure de notre stratégie, ici une liste de header
void *Strategy_Create(struct Cache *pcache)
{
    struct Cache_List* list=Cache_List_Create();
    for (int i = 0; i < pcache->nblocs; i++) {
        if (pcache->headers[i].flags == VALID) {
            Cache_List_Append(liste, pcache->headers[i]);
        }
    }
    // srand((unsigned int)time(NULL));
    return list;
}

void Strategy_Invalidate(struct Cache *pcache){

    /*
    struct Cache_List*list=(struct Cache_List*)pcache->pstrategy;
    struct Cache_List* tmp=list;
    struct Cache_List* next=list->next;
    while(tmp!=null){
        next=tmp->next;
        free(tmp);
        tmp=next;
    }
     */
    pcache->pstrategy=Cache_List_Clear((struct Cache_List*)pcache->pstrategy);
    //pcache->pstrategy=Strategy_Create(*cache);
}
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache){
    /*
    for (int i = 0; i < pcache->nblocs; i++) {
        if (pcache->headers[i].flags == VIDE) {
            return pcache->headers[i];
        }
    }
     */
    struct Cache_Block_Header* res=Cache_List_Remove_First((struct Cache_List*)pcache->pstrategy);
    return res;
}

void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb){
    struct Cache_List* tmp=list;
    while(tmp!=null){
        if(tmp->pheader==pb){
            Cache_List_Move_To_End(pb);
            return;
        }
        tmp=tmp->next;
    }
    Cache_List_Append(pb);
}

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb){
    struct Cache_List* tmp=list;
    while(tmp!=null){
        if(tmp->pheader==pb){
            Cache_List_Move_To_End(pb);
            return;
        }
        tmp=tmp->next;
    }
    Cache_List_Append(pb);
}
void Strategy_Close(struct Cache *pcache){
    Cache_List_Delete((struct Cache_List *)pcache->pstrategy);
}

char *Strategy_Name(){
    return "LRU";
}
