#include <stdlib.h>
#include <stdio.h>
#include "cache.h"
#include "low_cache.h"


//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){

    struct Cache* cache = (struct Cache*) malloc(sizeof(struct Cache));
    cache->file = fic;
    cache->fp = fopen(fic, "r" );
    cache->nblocks = nblocks;
    cache->nrecords = nrecords;
    cache->recordsz = recordsz;
    cache->blocksz = recordsz * nrecords;
    cache->nderef = nderef;
    cache->pstrategy = NULL;

    //init instrument
    struct Cache_Instrument* cacheInstrument = (Cache_Instrument*) malloc(sizeof(struct Cache_Instrument));
    cacheInstrument->n_reads=0;
    cacheInstrument->n_writes=0;
    cacheInstrument->n_hits=0;
    cacheInstrument->n_syncs=0;
    cacheInstrument->n_deref=0;

    //init Headers
    struct Cache_Block_Header* headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);
    cache->pfree = headers;
    cache->headers = headers;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){

}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){

}

//! Invalidation du cache.
Cache_Error Cache_Invalidate(struct Cache *pcache){

}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){

}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord){

}


//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache){

}