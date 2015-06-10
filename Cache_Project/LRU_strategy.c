#include <assert.h>
#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h"




//Renvoie la structure de notre stratégie, ici une liste de header
void *Strategy_Create(struct Cache *pcache)
{
   // struct Cache_List* list=Cache_List_Create();
    /*for (int i = 0; i < pcache->nblocks; i++) {
        if (pcache->headers[i].flags == VALID) {
            Cache_List_Append((struct Cache_List*)pcache->pstrategy, &(pcache->headers[i]));
        }
    }
     */
    return Cache_List_Create();
}

void Strategy_Invalidate(struct Cache *pcache){

    Cache_List_Clear((struct Cache_List*)pcache->pstrategy);
}
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache){
    for (int i = 0; i < pcache->nblocks; i++) {
        if (pcache->headers[i].flags == 0x0) {
            return &(pcache->headers[i]);
        }
    }
    struct Cache_Block_Header* res=Cache_List_Remove_First((struct Cache_List*)pcache->pstrategy);
    return res;
}

void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb){

    Cache_List_Move_To_End((struct Cache_List*)pcache->pstrategy,pb);
}

void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb){
    /*
    struct Cache_List* tmp=list;
    while(tmp!=null){
        if(tmp->pheader==pb){
            Cache_List_Move_To_End(pcache->pstrategy,pb);
            return;
        }
        tmp=tmp->next;
    }
    Cache_List_Append(pb);
     */
    Cache_List_Move_To_End((struct Cache_List*)pcache->pstrategy,pb);
}

void Strategy_Close(struct Cache *pcache){
    Cache_List_Delete((struct Cache_List *)pcache->pstrategy);
}

char *Strategy_Name(){
    return "LRU";
}
