#include "thread_pool.h"


static void thread_pool_exit_handler(void *data);
static void *thread_pool_cycle(void *data);
static int_t thread_pool_init_default(thread_pool_t *tpp, char *name);



static uint_t       thread_pool_task_id;

static int debug = 0;

//线程池初始化
thread_pool_t* thread_pool_init()
{
    int             err;
    pthread_t       tid;
    uint_t          n;
    pthread_attr_t  attr;
	thread_pool_t   *tp=NULL;

	tp = calloc(1,sizeof(thread_pool_t));

	if(tp == NULL){
	    fprintf(stderr, "thread_pool_init: calloc failed!\n");
	}

	thread_pool_init_default(tp, NULL);

    thread_pool_queue_init(&tp->queue);

    if (thread_mutex_create(&tp->mtx) != OK) {
		free(tp);
        return NULL;
    }

    if (thread_cond_create(&tp->cond) != OK) {
        (void) thread_mutex_destroy(&tp->mtx);
		free(tp);
        return NULL;
    }

    err = pthread_attr_init(&attr);
    if (err) {
        fprintf(stderr, "pthread_attr_init() failed, reason: %s\n",strerror(errno));
		free(tp);
        return NULL;
    }

    err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (err) {
        fprintf(stderr, "pthread_attr_setdetachstate() failed, reason: %s\n",strerror(errno));
		free(tp);
        return NULL;
    }


    for (n = 0; n < tp->threads; n++) {
        err = pthread_create(&tid, &attr, thread_pool_cycle, tp);
        if (err) {
            fprintf(stderr, "pthread_create() failed, reason: %s\n",strerror(errno));
			free(tp);
            return NULL;
        }
    }

    (void) pthread_attr_destroy(&attr);

    return tp;
}


void thread_pool_destroy(thread_pool_t *tp)
{
    uint_t           n;
    thread_task_t    task;
    volatile uint_t  lock;

    memset(&task,'\0', sizeof(thread_task_t));

    task.handler = thread_pool_exit_handler;
    task.ctx = (void *) &lock;

    for (n = 0; n < tp->threads; n++) {
        lock = 1;

        if (thread_task_post(tp, &task) != OK) {
            return;
        }

        while (lock) {
            sched_yield();
        }

        //task.event.active = 0;
    }

    (void) thread_cond_destroy(&tp->cond);
    (void) thread_mutex_destroy(&tp->mtx);

	free(tp);
}


static void
thread_pool_exit_handler(void *data)
{
    uint_t *lock = data;

    *lock = 0;

    pthread_exit(0);
}


//size为要执行的任务函数的参数大小
thread_task_t *
thread_task_alloc(size_t size)
{
    thread_task_t  *task;

    //任务内存+函数参数内存
    task = calloc(1,sizeof(thread_task_t) + size);
    if (task == NULL) {
        return NULL;
    }

    //task为thread_task_t指针
    task->ctx = task + 1;//task+1，此时指向的是任务函数的参数所在内存。

    return task;
}


//往线程池中投递任务
//线程池,线程任务
int_t
thread_task_post(thread_pool_t *tp, thread_task_t *task)
{
    if (thread_mutex_lock(&tp->mtx) != OK) {//上锁
        return ERROR;
    }

    //独立占有线程池结构

    if (tp->waiting >= tp->max_queue) {//线程池等待任务队列是否达到极限
        (void) thread_mutex_unlock(&tp->mtx);

        fprintf(stderr,"thread pool \"%s\" queue overflow: %ld tasks waiting\n",
                      tp->name, tp->waiting);
        return ERROR;
    }

    //task->event.active = 1;

    task->id = thread_pool_task_id++;//任务id++
    task->next = NULL;

    //发送一个信号，
    if (thread_cond_signal(&tp->cond) != OK) {
        (void) thread_mutex_unlock(&tp->mtx);
        return ERROR;
    }

    //一开始的时候last，默认指向的值first的地址。
    //所以此时给*tp->queue.last赋值后，first = tast
    //返回last为空，还是尾插法。
    *tp->queue.last = task;
    tp->queue.last = &task->next;

    tp->waiting++;

    (void) thread_mutex_unlock(&tp->mtx);

    if(debug)fprintf(stderr,"task #%lu added to thread pool \"%s\"\n",
                   task->id, tp->name);

    return OK;
}


static void *
thread_pool_cycle(void *data)
{
    thread_pool_t *tp = data;//所在线程池

    int                 err;
    thread_task_t       *task;


    if(debug)fprintf(stderr,"thread in pool \"%s\" started\n", tp->name);

   

    for ( ;; ) {
        if (thread_mutex_lock(&tp->mtx) != OK) {
            return NULL;
        }

        
        tp->waiting--;

        while (tp->queue.first == NULL) {
            if (thread_cond_wait(&tp->cond, &tp->mtx)
                != OK)
            {
                (void) thread_mutex_unlock(&tp->mtx);
                return NULL;
            }
        }

        task = tp->queue.first;
        tp->queue.first = task->next;

        if (tp->queue.first == NULL) {
            tp->queue.last = &tp->queue.first;
        }
		
        if (thread_mutex_unlock(&tp->mtx) != OK) {
            return NULL;
        }



        if(debug) fprintf(stderr,"run task #%lu in thread pool \"%s\"\n",
                       task->id, tp->name);

        task->handler(task->ctx);

        if(debug) fprintf(stderr,"complete task #%lu in thread pool \"%s\"\n",task->id, tp->name);

        task->next = NULL;

        free(task);
    }
}




static int_t
thread_pool_init_default(thread_pool_t *tpp, char *name)
{
	if(tpp)
    {
        tpp->threads = DEFAULT_THREADS_NUM;
        tpp->max_queue = DEFAULT_QUEUE_NUM;
            
        
		tpp->name = strdup(name?name:"default");
        if(debug)fprintf(stderr,
                      "thread_pool_init, name: %s ,threads: %lu max_queue: %ld\n",
                      tpp->name, tpp->threads, tpp->max_queue);

        return OK;
    }

    return ERROR;
}




