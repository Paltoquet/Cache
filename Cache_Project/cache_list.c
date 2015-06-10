

#define PRED(l) (l->pred)

struct Cache_List *Cache_List_Create(){
    struct Cache_List* list=(struct Cache_List*)malloc(sizeof(struct Cache_List));
    list->next=list;
    list->pheader=NULL;
    list->prev=list;
    return list;
}
/*
 * insertion à la fin
 */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* nouv=(struct Cache_List*)malloc(Cache_List);
    nouv->pheader=*pbh;
    nouv->next=list;
    nouv->pred=list->pred;
    list->pred=nouv;
    return liste;
}

void Cache_List_Clear(struct Cache_List *list) {
    struct Cache_List *list = (struct Cache_List *) pcache->pstrategy;
    struct Cache_List *tmp = list;
    struct Cache_List *next;
    while (next != list) {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
    list=Cache_List_Create();
}

void Cache_List_Delete(struct Cache_List *list){
    struct Cache_List *list = (struct Cache_List *) pcache->pstrategy;
    struct Cache_List *tmp = list;
    struct Cache_List *next;
    //free toute la mémoire de la liste
    while (next !=list) {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* tmp=list;
    tmp->pred=list;
    list->pheader=*pbh;
    list->next=tmp;

}
void Cache_List_Move_To_End(struct Cache_List *list,struct Cache_Block_Header *pbh){
    struct Cache_List* tmp=list;
    struct Cache_List* next=tmp->next;
    //parcours la liste pour voir si le bloque existe
    while(next!=list){
        //si il existe on le "supprime"
        if(tmp->pheader==pb){
            (tmp->pred)->next=tmp->next;
            (tmp->next)->pred=tmp->pred;
            break;
        }
        tmp=tmp->next;
    }
    //on l'ajoute à la fin
    Cache_List_Append(pb);
}

struct Cache_Block_Header *Cache_List_Remove_First(struct Cache_List *list){
    struct Cache_Block_Header* res=list->pheader;
    struct Cache_List* tmp=list->pred;
    list=list->next;
    list->pred=tmp;
    return res;
}

struct Cache_Block_Header *Cache_List_Remove_Last(struct Cache_List *list){
    struct Cache_Block_Header* res;
    res=(list->pred)->pheader;

}