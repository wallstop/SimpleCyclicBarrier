/*
 * CyclicBarrier.c
 *
 *  Created on: Nov 8, 2014
 *      Author: wallstop
 */

#include "CyclicBarrier.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static void cyclicBarrierFree(CyclicBarrier* this)
{
    pthread_mutex_destroy(&(this->threadCountLock));
    pthread_cond_destroy(&(this->threadCountComplete));
    free(this);
}

static void cyclicBarrierAwait(struct _CyclicBarrier* this)
{
    pthread_mutex_lock(&(this->threadCountLock));
    (this->currentThreadCount)--;
    if(this->currentThreadCount != 0)
    {
        const int error = pthread_cond_wait(&(this->threadCountComplete), &(this->threadCountLock));
        if (error)
            perror("pthread_con_wait");
    }
    else
    {
        this->currentThreadCount = this->initialThreadCount;
        const int error = pthread_cond_broadcast(&(this->threadCountComplete));
        if (error)
            perror("pthread_con_broadcast");
    }
    pthread_mutex_unlock(&(this->threadCountLock));
}

CyclicBarrier* newCyclicBarrier(const unsigned int threadCount)
{
    CyclicBarrier* this = (CyclicBarrier*) malloc(sizeof(CyclicBarrier));
    this->initialThreadCount = threadCount;
    this->currentThreadCount = threadCount;
    pthread_mutex_init(&(this->threadCountLock), NULL);
    pthread_cond_init(&(this->threadCountComplete), NULL);
    this->await = &cyclicBarrierAwait;
    this->free = &cyclicBarrierFree;

    return this;
}
