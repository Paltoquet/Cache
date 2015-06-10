#include <stdlib.h>
#include <stdio.h>
#include "cache.h"
#include "low_cache.h"
#include "strategy.h"


//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){

    struct Cache* cache = (struct Cache*) malloc(sizeof(struct Cache));
    cache->file = fic;
    cache->fp = fopen(fic, "r+" );
    cache->nblocks = nblocks;
    cache->nrecords = nrecords;
    cache->recordsz = recordsz;
    cache->blocksz = recordsz * nrecords;
    cache->nderef = nderef;

    //init instrument
    struct Cache_Instrument* cacheInstrument = (Cache_Instrument*) malloc(sizeof(struct Cache_Instrument));
    cacheInstrument->n_reads=0;
    cacheInstrument->n_writes=0;
    cacheInstrument->n_hits=0;
    cacheInstrument->n_syncs=0;
    cacheInstrument->n_deref=0;
    cache->instrument = cacheInstrument;

    //init Headers
    struct Cache_Block_Header* headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);
    cache->pfree = headers;
    cache->headers = headers;

    //strategie
    cache->pstrategy = Strategy_Create(cache);
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
    free(pcache->instrument);

    for( int i = 0; i < pcache->nblocks; i++){
        free(pcache->headers+i);
    }

    if( fclose(pcache->fp) == EOF ){
        return CACHE_KO;
    }

    Strategy_Close(pcache);

    free( pcache );

    return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
    struct Cache_Block_Header* header = NULL;
    for( int i = 0; i < pcache->nblocks; i++){

        if( (header = pcache->headers+i)->flags & MODIF ){

            if( fseek(pcache->fp, header->ibfile,SEEK_SET ) == EOF ){
                return CACHE_KO;
            }

            if( fputs(header->data, pcache->fp ) == EOF){
                return CACHE_KO;
            }

            header->flags =& ~MODIF;
        }
    }

    return CACHE_OK;
}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){
    for( int i = 0; i < pcache->nblocks; i++){
        pcache->headers[i].flags &= ~VALID;
    }

    Strategy_Invalidate(pcache);
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){

    struct Cache_Block_Header*  h = pcache->headers;
    for(int i = 0 ; h->ibfile != irfile && i < pcache->nblocks; h++, i++ );

    if( h->ibfile != irfile ){
        h = Strategy_Replace_Block(pcache);
    }

    strcpy(h->data, precord);

    Strategy_Read(pcache, h);

    return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){

}


//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache){

}