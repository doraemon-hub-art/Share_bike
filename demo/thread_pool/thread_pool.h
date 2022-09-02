#ifndef _THREAD_POOL_H_INCLUDED_
#define _THREAD_POOL_H_INCLUDED_


#include "thread.h"

//根据实际的应用情况来设置线程数量，CPU多少个核心，就多少个线程。
//线程池中线程的数量，不一定数量越多执行效率就越高，因为存在线程的切换。
//但是为了减少某些任务的阻塞时间，我们也可以开多一点线程，提高程序的吞吐量，以空间换时间。
#define DEFAULT_THREADS_NUM 4
//任务队列的最大容量
#define DEFAULT_QUEUE_NUM  65535


typedef unsigned long         atomic_uint_t;
typedef struct thread_task_s  thread_task_t;//线程任务
typedef struct thread_pool_s  thread_pool_t;//线程池


struct thread_task_s {
    thread_task_t       *next;//下一个任务
    uint_t               id;//任务ID
    void                *ctx;//上下文，任务要带的参数
    void               (*handler)(void *data);//函数指针，具体执行的任务。
};

typedef struct {
    thread_task_t        *first;
    thread_task_t        **last;//指向最后一个结点
} thread_pool_queue_t;//任务队列，单链表结构。

#define thread_pool_queue_init(q)                                         \
    (q)->first = NULL;                                                    \
    (q)->last = &(q)->first


struct thread_pool_s {
    pthread_mutex_t        mtx;         //互斥锁
    thread_pool_queue_t   queue;        //任务队列
    int_t                 waiting;      //线程池中没有处理的任务还有多少
    pthread_cond_t         cond;        //线程条件变量

    char                  *name;        //线程池的名字
    uint_t                threads;      //线程池中线程的数量
    int_t                 max_queue;    //任务队列最多能够容纳多少个任务
};

thread_task_t *thread_task_alloc(size_t size);
int_t thread_task_post(thread_pool_t *tp, thread_task_t *task);
thread_pool_t* thread_pool_init();
void thread_pool_destroy(thread_pool_t *tp);


#endif /* _THREAD_POOL_H_INCLUDED_ */
