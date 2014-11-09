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
    printf("%d \n", count++);   // Conveniently also lock printf. This serializes our prints to strictly increasing.
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

int cleanExit(CyclicBarrier* barrier, const int errorCode)
{
    barrier->free(barrier);
    return errorCode;
}

int main(int argc, char** argv)
{
    const unsigned int numThreads = 4;
    CyclicBarrier* barrier = newCyclicBarrier(numThreads);
    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * numThreads);

    {
        unsigned int i = 0; // Ugh, for C STD < C99
        for (i = 0; i < numThreads; ++i)
        {
            pthread_t* currentThread = &(threads[i]);
            const int ret = pthread_create(currentThread, NULL, &doAThing, barrier);
            if (ret != 0)
            {
                printf("Creating pthread %d caused an error!", i);
                return cleanExit(barrier, ret);
            }
        }
        for (i = 0; i < numThreads; ++i)
        {
            pthread_t* currentThread = &(threads[i]);
            const int ret = pthread_join(*currentThread, NULL);
            if (ret != 0)
            {
                printf("Joining pthread %d caused an error!", i);
                return cleanExit(barrier, ret);
            }
        }
    }
    return cleanExit(barrier, 0);
}
