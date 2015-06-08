//! Creation et initialisation de la stratégie (invoqué par la création de cache).
void *Strategy_Create(struct Cache *pcache){
    pcache->pstrategy = NULL;
    for( struct Cache_Block_Header *i = pcache->headers;i != NULL; i++ ) {
        i->flags = Cache_Flag.VIDE;
    }
    return NULL;
}

//! Fermeture de la stratégie.
void Strategy_Close(struct Cache *pcache){
    pcache->pstrategy = NULL;
    for( struct Cache_Block_Header *i = pcache->headers;i != NULL; i++ ) {
        i->flags = Cache_Flag.VIDE;
    }
}

//! Fonction "réflexe" lors de l'invalidation du cache.
void Strategy_Invalidate(struct Cache *pcache){
    pcache->pstrategy = NULL;
    for( struct Cache_Block_Header *i = pcache->headers;i != NULL; i++ ) {
        i->flags = Cache_Flag.VIDE;
    }
}

//! Algorithme de remplacement de bloc.
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache){

    //int* r = (int*) pcache->pstrategy;

}

//! Fonction "réflexe" lors de la lecture.
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb){

}

//! Fonction "réflexe" lors de l'écriture.
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb){

}

//! Identification de la stratégie.
char *Strategy_Name(){
    char* name = (char*)malloc(sizeof(char)*4);
    strcpy(name, "NUR");
    return name;
}