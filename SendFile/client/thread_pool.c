//
//
// a simple thread pool 
//
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "thread_pool.h"

static tpool_t *tpool = NULL;

//pthread process func
static void* thread_routine(void *arg)
{
    tpool_work_t *work;

    while(1) {
        //thread block when the task list is empty
        pthread_mutex_lock(&tpool->queue_lock);
        while(!tpool->queue_head && !tpool->shutdown) {
            pthread_cond_wait(&tpool->queue_ready, &tpool->queue_lock);
        }

		//check pool status,if shutdown,destroy the pool
        if (tpool->shutdown) {
            pthread_mutex_unlock(&tpool->queue_lock);
            pthread_exit(NULL);
        }

		//
        work = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        pthread_mutex_unlock(&tpool->queue_lock);
        work->routine(work->arg);

		//destroy the pool
		free(work->arg);
        free(work);
    }
    return NULL; 
}

//create thread pool
int tpool_create(int max_thr_num)
{
    int i;

	//create pool_struct
    tpool = calloc(1, sizeof(tpool_t));
    if (!tpool) {
        printf("%s: calloc tpool failed\n", __FUNCTION__);
        exit(1);
    }

    //initial the task list,mutex
    tpool->max_thr_num = max_thr_num;
    tpool->shutdown = 0;
    tpool->queue_head = NULL;
	tpool->queue_tail = NULL;
    if (pthread_mutex_init(&tpool->queue_lock, NULL) !=0) {
        printf("%s: pthread_mutex_init failed, errno:%d, error:%s\n",
            __FUNCTION__, errno, strerror(errno));
        exit(-1);
    }
    if (pthread_cond_init(&tpool->queue_ready, NULL) !=0 ) {
        printf("%s: pthread_cond_init failed, errno:%d, error:%s\n",
            __FUNCTION__, errno, strerror(errno));
        exit(-1);
    }

    //create work thread
    tpool->thr_id = calloc(max_thr_num, sizeof(pthread_t));
    if (!tpool->thr_id) {
        printf("%s: calloc thr_id failed\n", __FUNCTION__);
        exit(1);
    }
    for (i = 0; i < max_thr_num; ++i) {
        if (pthread_create(&tpool->thr_id[i], NULL, thread_routine, NULL) != 0){
            printf("%s:pthread_create failed, errno:%d, error:%s\n", __FUNCTION__, errno, strerror(errno));
            exit(-1);
        }
    }
    return 0;
}

//destroy the pool
void tpool_destroy()
{
    int i;
    tpool_work_t *member;

    if (tpool->shutdown) {
        return;
    }
	//set the pool status
    tpool->shutdown = 1;

    //wake all the therads
    pthread_mutex_lock(&tpool->queue_lock);
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);

	//thread gc
    for (i = 0; i < tpool->max_thr_num; ++i) {
        pthread_join(tpool->thr_id[i], NULL);
    }

	//free all the thread 
    free(tpool->thr_id);

	//free the running work thread
    while(tpool->queue_head) {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
		free(member->arg);
        free(member);
    }

	//destroy the mutex ,cond
    pthread_mutex_destroy(&tpool->queue_lock);
    pthread_cond_destroy(&tpool->queue_ready);

	//free pool struct
    free(tpool);
}

//add task to the list
int tpool_add_work(void*(*routine)(void*), void *arg)
{
	//the work list poionter
    tpool_work_t *work;

    if (!routine){
        printf("%s:Invalid argument\n", __FUNCTION__);
        return -1;
    }

    work = malloc(sizeof(tpool_work_t));
    if (!work) {
        printf("%s:malloc failed\n", __FUNCTION__);
        return -1;
    }
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;

	//add task to the work list
    pthread_mutex_lock(&tpool->queue_lock);
	//when the list is empty at first
    if ( !tpool->queue_head ) {
		//printf("first work in work-queue\n");
        tpool->queue_head = work;
		tpool->queue_tail = work;
    }
	//if the list not empty,query the tail of list
	else {
		//printf("not first work in work-queue\n");
		tpool->queue_tail->next=work;
		tpool->queue_tail=work;
  }
    //broadcast all thread,new task added
    pthread_cond_signal(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
    return 0;
}
