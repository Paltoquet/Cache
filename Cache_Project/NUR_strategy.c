
#include "low_cache.h"
#include "strategy.h"

typedef struct S_Data_NRU {
    int compteur;
    int nderef;
}Data_NRU;

static Data_NRU data_nru = {0,0};

void init_all_flag(struct Cache *pcache);
void init_flag(struct Cache *pcache, int flag);

//Permet de remettre les flags des caches à l'état VIDE
void init_all_flag(struct Cache *pcache){
    struct Cache_Block_Header *headers = pcache->headers;

    for( int i = 0;i < pcache->nblocks; i++ ) {
        headers[i].flags = 0x0;
    }
}

//Permet de mettre a 0 le flag en parametre dans tous les headerss du cache.
//flag: le n ieme bit sur le flag.
void init_flag(struct Cache *pcache, int flag){
    struct Cache_Block_Header *headers = pcache->headers;

    for( int i = 0;i < pcache->nblocks; i++ ) {
        headers[i].flags &= ~flag;
    }
}

//! Creation et initialisation de la stratégie (invoqué par la création de cache).
void *Strategy_Create(struct Cache *pcache){
    data_nru.compteur = 0;
    data_nru.nderef = pcache->nderef;
    pcache->pstrategy = &data_nru;
    init_all_flag(pcache);
    return &data_nru;
}

//! Fermeture de la stratégie.
void Strategy_Close(struct Cache *pcache){
    data_nru.compteur = 0;
    data_nru.nderef = 0;
    pcache->pstrategy = NULL;
    init_all_flag(pcache);
}

//! Fonction "réflexe" lors de l'invalidation du cache.
void Strategy_Invalidate(struct Cache *pcache){
    init_flag(pcache, REFER);
}

//! Algorithme de remplacement de bloc.
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache){
    struct Cache_Block_Header *headerMin = NULL;
    int nbMin;

    struct Cache_Block_Header *headers = pcache->headers;
    for( int i = 0; i < pcache->nblocks; i++ ) {
        int R =  (headers[i].flags & REFER ) == REFER;
        int M =  (headers[i].flags & MODIF ) == MODIF;
        int nb = 2*R+M;

        if( headerMin == NULL || nb < nbMin ){
            headerMin = headers+i;
            nbMin = nb;
        }

    }
    return headerMin;
}

//! Fonction "réflexe" lors de la lecture.
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb){
    data_nru.compteur++;
    pb->flags |= REFER;

    //Remise à zero si nderef est atteint
    if( data_nru.compteur == data_nru.nderef ){
        init_flag(pcache, REFER);
        data_nru.compteur = 0;
    }
}

//! Fonction "réflexe" lors de l'écriture.
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb){
    data_nru.compteur++;

    pb->flags |= REFER;

    //Remise à zero si nderef est atteint
    if( data_nru.compteur == data_nru.nderef ){
        init_flag(pcache, REFER);
        data_nru.compteur = 0;
    }
}

//! Identification de la stratégie.
char *Strategy_Name(){
    return "NUR";
}