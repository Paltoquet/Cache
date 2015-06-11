#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cache.h"
#include "low_cache.h"
#include "strategy.h"


static int compteur;

//! Création du cache.
struct Cache *Cache_Create(const char *fic, unsigned nblocks, unsigned nrecords,
                           size_t recordsz, unsigned nderef){

    compteur = 0;

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
    cache->instrument.n_deref= 0;

    //init Headers
    struct Cache_Block_Header* headers = (struct Cache_Block_Header*) malloc(sizeof(struct Cache_Block_Header)*nblocks);

    /*for (int i = 0; i < nblocks; i++){

    }*/
    cache->pfree = headers;
    cache->headers = headers;

    //strategie
    cache->pstrategy = Strategy_Create(cache);


    return cache;
}

//! Fermeture (destruction) du cache.
Cache_Error Cache_Close(struct Cache *pcache){

    if( Cache_Sync(pcache) == CACHE_KO ){
        return CACHE_KO;
    }

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
    printf("1 \n");
    int t=0;
    pcache->instrument.n_reads ++;
    pcache->instrument.n_hits ++;
    int k;
    struct Cache_Block_Header*  h = pcache->headers;
    for(int i = 0 ; !t ; h++, i++ ){
        for(k=0;k<pcache->nrecords;k++){
            if(h->ibfile+k==irfile){
                t=1;break;
            }
        }
        if(i==(pcache->nblocks-1)){
            t=1;
        }
    }
    printf("fin 1,5 \n");
    //ON l a trouvé
    if( h->ibfile == irfile-k && h->data != NULL){
        printf("fin 1.7 \n");
        h->flags |= VALID;
        printf("%p %p %p \n", precord, (h->data+k), pcache->recordsz);
        memcpy(precord, (h->data)+pcache->recordsz*k,pcache->recordsz);
        Strategy_Read(pcache, h);
        compteur++;
        printf("fin 1.8 \n");
        if( compteur == NSYNC ){
            Cache_Sync(pcache);
            compteur = 0;
        }

        return CACHE_OK;
    }

    printf(" 2 \n");
    //Cherche bloque libre
    h = Strategy_Replace_Block(pcache);
    if (fseek(pcache->fp, irfile, SEEK_SET) == EOF) {
        return CACHE_KO;
    }


    if( h->data != NULL ) free(h->data);
    h->data = (char*)malloc(pcache->nrecords*pcache->recordsz);
    char buffer[pcache->recordsz];
    for( int i = 0; i < pcache->nrecords; i++){
        if (fgets(buffer, pcache->recordsz, pcache->fp) == NULL) {
            return CACHE_KO;
        }
        strcat(h->data, buffer);
    }

    h->ibfile = irfile;
    h->flags &= ~MODIF;

    h->flags &= VALID;
    memcpy(precord, h->data,pcache->recordsz);
    Strategy_Read(pcache, h);

    //NSYNC
    compteur++;
    if (compteur == NSYNC) {
        Cache_Sync(pcache);
        compteur = 0;
    }

    printf("fin 1 \n");
    return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord)
{
    pcache->instrument.n_writes ++;
    pcache->instrument.n_hits ++;

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

            next->flags &= MODIF ;
            next->flags &= VALID ;
            is_in_cache = 1 ;
        }

        next++;
    }

    if(!is_in_cache)
    {
        pcache->instrument.n_hits --;
        next = Strategy_Replace_Block(pcache);

        strcpy(next->data,precord);

        next->flags &= VALID;
        next->flags &= MODIF;
        next->ibfile = irfile;
    }

    //appel a write de la strategie utilise
    Strategy_Write(pcache,next);

    //NSYNC
    compteur++;
    if( compteur == NSYNC ){
        Cache_Sync(pcache);
        compteur = 0;
    }

    return CACHE_OK;
}


//! Résultat de l'instrumentation.
struct Cache_Instrument *Cache_Get_Instrument(struct Cache *pcache)
{

    Cache_Sync(pcache);
    compteur = 0;

    /* Retourne une copie de la structure d’instrumentation du cache pointé par pcache .
             Attention : tous les compteurs de la structure courante sont remis à 0 par cette
     fonction.*/
    struct Cache_Instrument * inst =  (struct Cache_Instrument*) malloc(sizeof(struct Cache_Instrument));
    inst->n_hits = pcache->instrument.n_hits;
    inst->n_reads = pcache->instrument.n_reads;
    inst->n_syncs = pcache->instrument.n_syncs;
    inst->n_writes = pcache->instrument.n_writes;
    inst->n_deref = pcache->instrument.n_deref;


    pcache->instrument.n_hits = 0;
    pcache->instrument.n_reads = 0;
    pcache->instrument.n_syncs = 0;
    pcache->instrument.n_writes = 0;
    pcache->instrument.n_deref = 0;

    return inst;
}
