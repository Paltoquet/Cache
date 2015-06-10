
#include "low_cache.h"

struct Cache_Block_Header *Get_Free_Block(struct Cache *pcache){

    if( pcache->pfree != NULL ) return pcache->pfree;

    for(int i = 0; i < pcache->nblocks; i++){

        if( pcache->headers[i].flags & VALID ){
            return pcache->headers+i;
        }
    }

    return NULL;
}