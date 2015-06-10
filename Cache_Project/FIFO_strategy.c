#include "cache_list.h"
#include "low_cache.h"



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
    return Cache_List_Create();
}

//! Fermeture de la stratégie.
void Strategy_Close(struct Cache *pcache)
{
    // detruit le contenu
    Cache_List_Delete((struct Cache_List *) pcache->pstrategy);
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
    struct Cache_List * blocks = (struct Cache_List *) pcache->pstrategy;
    struct Cache_Block_Header * block = Get_Free_Block(pcache);

    if(block!=NULL)
    {
        Cache_List_Append( blocks, block);
        return block;
    }
    block = Cache_List_Remove_First(blocks);
    //on le déplace en bout de ligne
    Cache_List_Append(blocks, block);
    //return le plus vieux bloc (premier element de la liste pstrategy)
    return block;
}

//! Fonction "réflexe" lors de la lecture.
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pb)
{
    //rien à faire
}

//! Fonction "réflexe" lors de l'écriture.
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pb)
{
    // on écrit sur un block, le bloc change d'affection
    // FIFO => deplacement du bloc en queue de list.
    Cache_List_Move_To_End( (struct Cache_List *) pcache->pstrategy , pb );
}

//! Identification de la stratégie.
char *Strategy_Name()
{
    return "FIFO";
}