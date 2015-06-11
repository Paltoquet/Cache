#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cache.h"
#include "low_cache.h"
#include "strategy.h"




//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){

    struct Cache* cache = (struct Cache*) malloc(sizeof(struct Cache));
    cache->file = (char*)malloc( sizeof(char) * (strlen(fic)+1) );
    strcpy(cache->file, fic);
    cache->fp = fopen(fic, "r+" );
    cache->nblocks = nblocks;
    cache->nrecords = nrecords;
    cache->recordsz = recordsz;
    cache->blocksz = recordsz * nrecords;
    cache->nderef = nderef;

    //init instrument
    cache->instrument.n_reads=0;
    cache->instrument.n_writes=0;
    cache->instrument.n_hits=0;
    cache->instrument.n_syncs=0;
    cache->instrument.n_deref=0;

    //init Headers
    struct Cache_Block_Header* headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);
    cache->pfree = headers;
    cache->headers = headers;

    //strategie
    cache->pstrategy = Strategy_Create(cache);


    return cache;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){
    free(pcache->file);
    free(pcache->headers);

    if( fclose(pcache->fp) == EOF ){
        return CACHE_KO;
    }

    Strategy_Close(pcache);
    free( pcache );
    return CACHE_OK;
}

//! Synchronisation du cache.
Cache_Error Cache_Sync(struct Cache *pcache){
    pcache->instrument.n_syncs ++;

    struct Cache_Block_Header* header = NULL;
    for( int i = 0; i < pcache->nblocks; i++){

        if( (header = pcache->headers+i)->flags & MODIF ){

            if( fseek(pcache->fp, header->ibfile,SEEK_SET ) == EOF ){
                return CACHE_KO;
            }

            if( fputs(header->data, pcache->fp ) == EOF){
                return CACHE_KO;
            }

            header->flags &= ~MODIF;
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

    return CACHE_OK;
}

//! Lecture  (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){

    pcache->instrument.n_reads ++;
    pcache->instrument.n_hits ++;

    struct Cache_Block_Header*  h = pcache->headers;
    for(int i = 0 ; h->ibfile != irfile && i < pcache->nblocks; h++, i++ );

    if( h->ibfile != irfile ){
        pcache->instrument.n_hits --;
        h = Strategy_Replace_Block(pcache);
    }

    strcpy(precord, h->data);

    Strategy_Read(pcache, h);

    return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord)
{
    pcache->instrument.n_writes ++;

    // recherche du bloc irfile dans le cache
    struct Cache_Block_Header *tmp = pcache->headers;
    struct Cache_Block_Header *next = tmp;
    int block_courant = 0;
    int is_in_cache = 0;

    while ( !is_in_cache && block_courant++ < pcache->nblocks ) {

        if(next->ibfile == irfile)
        {
            // ecrit le contenu du buffer precords dans le block
            if( next->data != NULL) free(next->data);
            next->data = (char*)malloc(sizeof(char) * (strlen(precord)+1));

            strcpy(next->data,precord);
            //Modification des flags
            if(next->flags & 0) // le bloc est invalide
                next->flags = VALID ;

            if(next->flags & VALID) // le bloc possede des donnees et on ecrit dessus
                next->flags = MODIF ;
            is_in_cache = 1 ;


        }

        next++;
    }

    if(!is_in_cache)
    {
        next = Strategy_Replace_Block(pcache);
        strcpy(next->data,precord);
        //Modification des flags
        if(next->flags & 0) // le bloc est invalide
            next->flags = VALID ;

        if(next->flags & VALID) // le bloc possede des donnees et on ecrit dessus
            next->flags = MODIF ;
    }

    //appel a write de la strategie utilise
    Strategy_Write(pcache,next);
    return CACHE_OK;
}


//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache)
{

    /* Retourne une copie de la structure d’instrumentation du cache pointé par pcache .
             Attention : tous les compteurs de la structure courante sont remis à 0 par cette
     fonction.*/

    //TODO
    //probleme pointeur Cache_Instrument
    //compteur instrument deref

    struct Cache_Instrument * inst =  &pcache->instrument;
    pcache->instrument.n_deref = 0;
    pcache->instrument.n_hits = 0;
    pcache->instrument.n_reads = 0;
    pcache->instrument.n_syncs = 0;
    pcache->instrument.n_writes = 0;
    return inst;
}
