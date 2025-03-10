#ifndef _POSIX_PTHREAD_SEMANTICS
#define _POSIX_PTHREAD_SEMANTICS
#endif
#include "sthread.h"
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string.h>


void handle_pthread_error(const char* s, int rc)
{
    fprintf(stderr, "%s: %s\n", s, strerror(rc));
    exit(-1);
}

void smutex_init(smutex_t *mutex)
{
    int rc;

    if ((rc = pthread_mutex_init(mutex, NULL)))
        handle_pthread_error("pthread_mutex_init failed", rc);
}

void smutex_destroy(smutex_t *mutex)
{
    int rc; 

    if ((rc = pthread_mutex_destroy(mutex)))
        handle_pthread_error("pthread_mutex_destroy failed", rc);
}

void smutex_lock(smutex_t *mutex)
{
    int rc;

    if ((rc = pthread_mutex_lock(mutex)))
        handle_pthread_error("pthread_mutex_lock failed", rc);
}

void smutex_unlock(smutex_t *mutex)
{
    int rc;

    if ((rc = pthread_mutex_unlock(mutex)))
        handle_pthread_error("pthread_mutex_unlock failed", rc);
}


void scond_init(scond_t *cond)
{
    int rc;

    if ((rc = pthread_cond_init(cond, NULL)))
        handle_pthread_error("pthread_cond_init failed", rc);
}

void scond_destroy(scond_t *cond)
{
    int rc;

    if ((rc = pthread_cond_destroy(cond)))
        handle_pthread_error("pthread_cond_destroy failed", rc);
}

void scond_signal(scond_t *cond, smutex_t *mutex __attribute__((unused)))
{
    //
    // assert(mutex is held by this thread);
    //

    int rc;

    if ((rc = pthread_cond_signal(cond)))
        handle_pthread_error("pthread_cond_signal failed", rc);
}

void scond_broadcast(scond_t *cond, smutex_t *mutex __attribute__((unused)))
{
    //
    // assert(mutex is held by this thread);
    //
    int rc;

    if ((rc = pthread_cond_broadcast(cond)))
        handle_pthread_error("pthread_cond_broadcast failed", rc);
}

void scond_wait(scond_t *cond, smutex_t *mutex)
{
    //
    // assert(mutex is held by this thread);
    //
    
    int rc;

    if ((rc = pthread_cond_wait(cond, mutex)))
        handle_pthread_error("pthread_cond_wait failed", rc);
}



void sthread_create(sthread_t *thread,
                    void* (*start_routine)(void*), 
                    void *argToStartRoutine)
{
    //
    // When a detached thread returns from
    // its entry function, the thread will be destroyed.  If we
    // don't detach it, then the memory associated with the thread
    // won't be cleaned up until somebody "joins" with the thread
    // by calling pthread_wait().
    //

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    int rc;

    if ((rc = pthread_create(thread, &attr, start_routine, argToStartRoutine)))
        handle_pthread_error("pthread_create failed", rc);
}

void sthread_exit(void)
{
    pthread_exit(NULL);
}

void sthread_join(sthread_t thrd)
{
    pthread_join(thrd, NULL);
}


/*
 * WARNING:
 * Do not use sleep for synchronizing threads that 
 * should be waiting for events (using condition variables)!
 * Sleep should only be used to wait for a specified amount
 * of time! (If you find yourself looping on a predicate
 * and calling sleep in the loop, you probably are using
 * it incorrectly! We will deduct points from your grade
 * if you do this!
 */
void sthread_sleep(unsigned int seconds, unsigned int nanoseconds)
{
    struct timespec rqt;
    assert(nanoseconds < 1000000000);
    rqt.tv_sec  = seconds;
    rqt.tv_nsec = nanoseconds;
    if (nanosleep(&rqt, NULL) != 0)
    {
        perror("sleep failed. Woke up early");
        exit(-1);
    }
}



/*
 * random() in stdlib.h is not MT-safe, so we need to lock
 * it.
 */
pthread_mutex_t sulock = PTHREAD_MUTEX_INITIALIZER;

long sutil_random()
{
    long val;

    if (pthread_mutex_lock(&sulock))
    {
        perror("random lock");
        exit(-1);
    }
    val = random();
    if (pthread_mutex_unlock(&sulock))
    {
        perror("random unlock");
        exit(-1);
    }
    return val;
}
