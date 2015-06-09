


struct Cache_List *Cache_List_Create(){
    struct Cache_List* list=(struct Cache_List*)malloc(sizeof(struct Cache_List));
    list->next=NULL;
    list->pheader=NULL;
    list->prev=NULL;
    return list;
}
/*
 * insertion à la fin
 */
void Cache_List_Append(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* nouv=(struct Cache_List*)malloc(Cache_List);
    nouv->pheader=*pbh;
    nouv->next=NULL;
    nouv->prev=NULL;
    if(list->pheader==NULL){
        list=nouv;
        return;
    }
    struct Cache_List* tmp=list;
    struct Cache_List* pred;
    while(temp->next != NULL)
    {
        temp=temp->nxt;
    }
    nouv->prev=temp;
    temp->nxt = nouv;
    return liste;
}

void Cache_List_Clear(struct Cache_List *list) {
    struct Cache_List *list = (struct Cache_List *) pcache->pstrategy;
    struct Cache_List *tmp = list;
    struct Cache_List *next;
    while (tmp != null) {
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
    while (tmp != null) {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

void Cache_List_Prepend(struct Cache_List *list, struct Cache_Block_Header *pbh){
    struct Cache_List* nouv=(struct Cache_List*)malloc(Cache_List);
    struct Cache_List* tmp=list;
    nouv->pheader=*pbh;
    nouv->next=tmp;
    tmp->pred=nouv;
    list=nouv;

}
void Cache_List_Move_To_End(struct Cache_List *list,struct Cache_Block_Header *pbh){
    struct Cache_List* tmp=list;
    //parcours la liste pour voir si le bloque existe
    while(tmp!=null){
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