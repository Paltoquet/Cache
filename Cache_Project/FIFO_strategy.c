#include "low_cache.h"
#include "cache_list.h"
#include "strategy.h"


/*!
 * \file strategy.h
 *
 * \brief Interface utilisée par les fonctions de gestion de la stratégie de remplacement.
 *
 * \author Jean-Paul Rigault
 *
 * $Id: strategy.h,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */


/*!
 * \defgroup strategy_interface Interface de la stratégie de remplacement
 *
 * Ces fonctions sont utilisées par celles de l'API du cache pour gérer la
 * stratégie de remplacement. Elles sont définies dans les différents fichiers
 * de stratégie.
 *
 * @{
 */

//! Creation et initialisation de la stratégie (invoqué par la création de cache).
void *Strategy_Create(struct Cache *pcache)
{
    /*struct Cache_List* cl = Cache_List_Create();
        for(int i = 0; i < pcache->nblocks; i++) {
            if(pcache->headers[i]->flags == VALID) Cache_List_Append(cl, pcache->headers[i]);
        }
        return cl;*/
    return Cache_List_Create();
}

//! Fermeture de la stratégie.
void Strategy_Close(struct Cache *pcache)
{
    // detruit le contenu
    Cache_List_Delete(pcache->pstrategy);
    // libere la memoire associe a la list FIFO
    free(pcache->pstrategy);

}

//! Fonction "réflexe" lors de l'invalidation du cache.
void Strategy_Invalidate(struct Cache *pcache)
{
    //lorsque le cache est invalidate, tous les blocs deviennent invalides donc il n'y a plus d'ordre entre les blocs
    Cache_List_Clear((struct Cache_List*)pcache->pstrategy);
}

//! Algorithme de remplacement de bloc.
// Appel si le cache ne possède plus de block libre
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    Cache_List * blocks = pcache->pstrategy;
    Cache_Block_Header * block = &blocks[0];
    //on le déplace en bout de ligne
    Cache_List_Move_To_End((struct Cache_List *)blocks, (struct Cache_Block_Header *)block);
    //return le plus vieux bloc (premier element de la liste pstrategy)
    return block;
}

//! Fonction "réflexe" lors de la lecture.
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb)
{
    // deplace le block modifie a la fin de la liste
    Cache_List_Move_To_End( pcache->pstrategy , pb );

}

//! Fonction "réflexe" lors de l'écriture.
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb)
{
    Cache_List_Move_To_End( pcache->pstrategy , pb );
}

//! Identification de la stratégie.
char *Strategy_Name()
{
    malloc(sizeof("FIFO"));
    return "FIFO";
}
