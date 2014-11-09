/*
 * CyclicBarrier.h
 *
 *  Created on: Nov 8, 2014
 *      Author: wallstop
 */

#ifndef CYCLICBARRIER_H_
#define CYCLICBARRIER_H_

#include "pthread.h"

typedef struct _CyclicBarrier
{
    unsigned int    initialThreadCount;
    unsigned int    currentThreadCount;
    pthread_mutex_t threadCountLock;

    pthread_cond_t  threadCountComplete;

    void (*free)(struct _CyclicBarrier*);
    void (*await)(struct _CyclicBarrier*);

} CyclicBarrier;

CyclicBarrier* newCyclicBarrier(const unsigned int threadCount);


#endif /* CYCLICBARRIER_H_ */
