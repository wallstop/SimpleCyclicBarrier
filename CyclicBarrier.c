/*
 * CyclicBarrier.c
 *
 *  Created on: Nov 8, 2014
 *      Author: wallstop
 */

#include "CyclicBarrier.h"

#include <assert.h>
#include <stdlib.h>

void cyclicBarrierFree(CyclicBarrier* this)
{
    pthread_mutex_destroy(&(this->threadCountLock));
    pthread_mutex_destroy(&(this->barrierLock));
    pthread_cond_destroy(&(this->barrierComplete));
    free(this);
}

const unsigned int changeThreadCount(pthread_mutex_t* threadMutex,
        unsigned int* value, const unsigned int valueChanged)
{
    assert(NULL != threadMutex);
    assert(NULL != value);

    unsigned int result = 0;
    pthread_mutex_lock(threadMutex);
    *value = (*value + valueChanged);
    result = *value;
    pthread_mutex_unlock(threadMutex);
    return result;
}

void checkBarrierComplete(pthread_mutex_t* barrierLock,
        pthread_cond_t* barrierComplete, const unsigned int threadNumber,
        const unsigned int checkNumber)
{
    pthread_mutex_lock(barrierLock);
    if (threadNumber != checkNumber)
        pthread_cond_wait(barrierComplete, barrierLock);
    else
        pthread_cond_broadcast(barrierComplete);
    pthread_mutex_unlock(barrierLock);
}

void cyclicBarrierAwait(struct _CyclicBarrier* this)
{
    const unsigned int threadNumber = changeThreadCount(
            &(this->threadCountLock), &(this->currentThreadCount), -1);
    checkBarrierComplete(&(this->barrierLock), &(this->barrierComplete),
            threadNumber, 0);
    const unsigned int finishedThreadNumber = changeThreadCount(
            &(this->threadCountLock), &(this->currentThreadCount), 1);
    checkBarrierComplete(&(this->currentThreadCount),
            &(this->currentThreadCount), finishedThreadNumber,
            this->initialThreadCount);
}

CyclicBarrier* newCyclicBarrier(const unsigned int threadCount)
{
    CyclicBarrier* this = (CyclicBarrier*) malloc(sizeof(CyclicBarrier));
    this->currentThreadCount = this->initialThreadCount = threadCount;
    pthread_mutex_init(&(this->threadCountLock), NULL);
    pthread_mutex_init(&(this->barrierLock), NULL);
    pthread_cond_init(&(this->barrierComplete), NULL);
    this->await = &cyclicBarrierAwait;
    this->free = &cyclicBarrierFree;

    return this;
}
