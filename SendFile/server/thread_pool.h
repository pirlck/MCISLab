//
//
// a simple thread pool 
//
#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__


#include <pthread.h>

// worker node 
typedef struct tpool_work {
    void*               (*routine)(void*);       //call back func for work
    void                *arg;                    //call back func's para
    struct tpool_work   *next;
}tpool_work_t;

typedef struct tpool {
    int             shutdown;                    // pool status 
    int             max_thr_num;                 // max num of threads
    pthread_t       *thr_id;                     // thread array pr
    tpool_work_t    *queue_head;                 // head of task list
	tpool_work_t    *queue_tail; 				 // tail of task list
    pthread_mutex_t queue_lock;
    pthread_cond_t  queue_ready;
}tpool_t;

//
// brief     create thread pool
// param     max_thr_num :max size thread 
// return    0: success other: failed
//
int tpool_create(int max_thr_num);

//
// brief     destroy pool
//
void tpool_destroy();

//
// brief     add task for pool
// param     routine: call_back func
// param     arg call_back func para
// return    0: success other:failed
//
int tpool_add_work(void*(*routine)(void*), void *arg);

#endif



