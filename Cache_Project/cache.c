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
    cache->fp = fopen(fic, "a+" );
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

//! Lecture (à travers le cache).
Cache_Error Cache_Read(struct Cache *pcache, int irfile, void *precord){
    pcache->instrument.n_reads ++;
    pcache->instrument.n_hits ++;

    struct Cache_Block_Header *h = NULL;
    int i, j;
    for( i = 0; i < pcache->nblocks; i++){
        h = (pcache->headers)+i;
        for (j= 0;j < pcache->nrecords; j++){

            if( irfile == (h->ibfile)+j ){
                h+=j;
                break;
            }
        }

        if( h->ibfile == irfile )break;
    }

    //ON l a trouvé
    if( h->ibfile == irfile && h->data != NULL && h->flags & VALID){
        memcpy(precord, h->data ,pcache->recordsz);
    }
    else {//Cherche bloque libre
        pcache->instrument.n_hits --;
        char buffer[pcache->recordsz];
        h = Strategy_Replace_Block(pcache);
        if (fseek(pcache->fp, irfile, SEEK_SET) == EOF)return CACHE_KO;
        //on remplie les petit block
        for (int i = 0; i < pcache->nrecords; i++) { ;
            if (fgets(buffer, pcache->recordsz, pcache->fp) == NULL) return CACHE_KO;

            char a [(int)(pcache->nrecords * pcache->recordsz) / sizeof(char)];
            h[i].data = a;
            if( i == 0 ) strcat(precord, buffer);
            strcat(h[i].data, buffer);
            h[i].ibfile = irfile++;
        }


    }

    h->flags |= VALID;
    Strategy_Read(pcache, h);

    //NSYNC
    compteur++;
    if (compteur == NSYNC) {
        Cache_Sync(pcache);
        compteur = 0;
    }

    return CACHE_OK;
}

//! Écriture (à travers le cache).
Cache_Error Cache_Write(struct Cache *pcache, int irfile, const void *precord)
{
    pcache->instrument.n_writes ++;
    pcache->instrument.n_hits ++;

    struct Cache_Block_Header *h = NULL;
    int i, j;
    for( i = 0; i < pcache->nblocks; i++){
        h = (pcache->headers)+i;
        for (j= 0;j < pcache->nrecords; j++){

            if( irfile == (h->ibfile)+j ){
                h+=j;
                break;
            }
        }

        if( h->ibfile == irfile )break;

    }

    //ON l'a pas trouvé
    if( !(h->ibfile == irfile && h->data != NULL)  ){
        pcache->instrument.n_hits --;
        h = Strategy_Replace_Block(pcache);
        h->ibfile = irfile;

        char a [(int)(pcache->nrecords * pcache->recordsz) / sizeof(char)];
        h->data = a;
    }
    memcpy(h->data, precord ,pcache->recordsz);

    h->flags |= VALID;
    h->flags |= MODIF;
    Strategy_Write(pcache, h);

    //NSYNC
    compteur++;
    if (compteur == NSYNC) {
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
