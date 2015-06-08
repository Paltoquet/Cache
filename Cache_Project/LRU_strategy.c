#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "cache_list.h"




//Renvoie la structure de notre stratégie, ici une liste de header
void *Strategy_Create(struct Cache *pcache)
{
    struct Cache_List* list=Cache_List_Create();
    for (int i = 0; i < pcache->nblocs; i++) {
        if(pcache->headers[i].flags==VALID){
            Cache_List_Append(liste,pcache->headers[i]);
        }
    }
    pcache->pstrategy=list;
    // srand((unsigned int)time(NULL));
    return list;
}

void Strategy_Invalidate(struct Cache *pcache){
    struct Cache_List*list=(struct Cache_List*)pcache->pstrategy;
    struct Cache_List* tmp=list;
    struct Cache_List* next=list->next;
    while(tmp!=null){
        next=tmp->next;
        free(tmp);
        tmp=next;
    }
}