/*!
 * \file RAND_strategy.c
 *
 * \brief  Stratégie de remplacement au hasard..
 *
 * \author Jean-Paul Rigault
 *
 * $Id: RAND_strategy.c,v 1.3 2008/03/04 16:52:49 jpr Exp $
 */

#include <assert.h>

#include "strategy.h"
#include "low_cache.h"
#include "time.h"

/*!
 * FIFO : pas grand chose à faire ici.
 *
 * En fait, nous initialisons le germe
 * (seed) du générateur aléatoire à quelque chose d'éminemment variable, pour
 * éviter d'avoir la même séquence à chque exécution...
 */
void *Strategy_Create(struct Cache *pcache)
{
    // srand((unsigned int)time(NULL));
    struct Cache_List* cl = Cache_List_Create();
    for(int i = 0; i < pcache->nblocks; i++) {
        if(pcache->headers[i]->flags == VALID) Cache_List_Append(cl, pcache->headers[i]);
    }
    return cl;
}

/*!
 * FIFO : Rien à faire ici.
 */
void Strategy_Close(struct Cache *pcache)
{
    Cache_List_Delete(pcache->pstrategy);
}

/*!
 * FIFO : Rien à faire ici.
 */
void Strategy_Invalidate(struct Cache *pcache)
{
}

/*!
 * FIFO : On prend le premier bloc invalide. S'il n'y en a plus, on prend un bloc au hasard.
 */
struct Cache_Block_Header *Strategy_Replace_Block(struct Cache *pcache)
{
    int ib;
    struct Cache_Block_Header *pbh;

    /* On cherche d'abord un bloc invalide */
    if ((pbh = Get_Free_Block(pcache)) != NULL) return pbh;

    /* Sinon on tire un numéro de bloc au hasard */
    ib = RANDOM(0, pcache->nblocks);
    return &pcache->headers[ib];
}


/*!
 * FIFO : Rien à faire ici.
 */
void Strategy_Read(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

/*!
 * FIFO : Rien à faire ici.
 */
void Strategy_Write(struct Cache *pcache, struct Cache_Block_Header *pbh)
{
}

char *Strategy_Name()
{
    return "FIFO";
}
