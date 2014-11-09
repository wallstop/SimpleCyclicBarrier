/*
 * main.c
 *
 *  Created on: Nov 8, 2014
 *      Author: wallstop
 */

#include "CyclicBarrier.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

pthread_mutex_t mutex;
int count = 0;

void printCount()
{
    pthread_mutex_lock(&mutex);
    int internalCount = count = count + 1;
    printf("%d \n", internalCount);
    pthread_mutex_unlock(&mutex);
}

void* doAThing(void *args)
{
    assert(NULL != args);
    CyclicBarrier* barrier = (CyclicBarrier* )args;
    while (1)
    {
        printCount();
        barrier->await(barrier);
    }
}

int main(int argc, char** argv)
{
    const unsigned int numThreads = 4;
    CyclicBarrier* barrier = newCyclicBarrier(numThreads);
    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * numThreads);

    {
        unsigned int i = 0;
        for (i = 0; i < numThreads; ++i)
        {
            pthread_t* currentThread = &(threads[i]);
            int ret = pthread_create(currentThread, NULL, &doAThing, barrier);
            if (ret != 0)
            {
                printf("Creating pthread %d caused an error!", i);
                return ret;
            }
        }
        for (i = 0; i < numThreads; ++i)
        {
            pthread_t* currentThread = &(threads[i]);
            pthread_join(*currentThread, NULL);
        }
    }
    barrier->free(barrier);
    return 0;
}
