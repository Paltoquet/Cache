static int compteur;

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
        headers[i].flags &= ~(1 << (flag-1));
    }
}

//! Creation et initialisation de la stratégie (invoqué par la création de cache).
void *Strategy_Create(struct Cache *pcache){
    compteur = 0;
    pcache->pstrategy = NULL;
    init_all_flag(pcache);
    return NULL;
}

//! Fermeture de la stratégie.
void Strategy_Close(struct Cache *pcache){
    compteur = 0;
    pcache->pstrategy = NULL;
    init_all_flag(pcache);
}

//! Fonction "réflexe" lors de l'invalidation du cache.
void Strategy_Invalidate(struct Cache *pcache){
    compteur = 0;
    pcache->pstrategy = NULL;
    init_flag(pcache, REFER);
}

//! Algorithme de remplacement de bloc.
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache){
    struct Cache_Block_Header *headerMin = NULL;
    int nbMin;

    struct Cache_Block_Header *headers = pcache->headers;
    for( int i = 0; i < pcache->nblocks; i++ ) {
        int R =  (headers[i].flags & (1 << (REFER-1))) == REFER;
        int M =  (headers[i].flags & (1 << (MODIF-1))) == MODIF;
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
    compteur++;
    pb->flags |= REFER;

    //Remise à zero si nderef est atteint
    if( compteur == pcache->nderef ){
        init_flag(pcache, REFER);
        compteur = 0;
    }
}

//! Fonction "réflexe" lors de l'écriture.
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb){
    compteur++;

    pb->flags |= REFER;
    pb->flags |= MODIF;

    //Remise à zero si nderef est atteint
    if( compteur == pcache->nderef ){
        init_flag(pcache, REFER);
        compteur = 0;
    }
}

//! Identification de la stratégie.
char *Strategy_Name(){
    return "NUR";
}