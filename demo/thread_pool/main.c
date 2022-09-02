#include "thread_pool.h"

struct test{
	 int arg1;
	 int arg2;
};

void task_handler1(void* data){
   static int index = 0;
   printf("Hello, this is 1th test.index=%d\r\n", index++);

}

void task_handler2(void* data){
   static int index = 0;
   printf("Hello, this is 2th test.index=%d\r\n", index++);

}

void task_handler3(void* data){
   static int index = 0;
   struct test *t = (struct test *) data;
   
   printf("Hello, this is 3th test.index=%d\r\n", index++);
   printf("arg1: %d, arg2: %d\n", t->arg1, t->arg2);

}

int
main(int argc, char **argv)
{
   thread_pool_t* tp = NULL;//定义一个线程池指针
   int i = 0;
   
   tp = thread_pool_init(); //线程池初始化

   //分配任务内存
   thread_task_t * test1 = thread_task_alloc(0);
   thread_task_t * test2 = thread_task_alloc(0);
   thread_task_t * test3 = thread_task_alloc(sizeof(struct test));
   //指定任务
   test1->handler = task_handler1;
   test2->handler = task_handler2;
   test3->handler = task_handler3;

   //通过结构体指定参数
   ((struct test*)test3->ctx)->arg1 = 666;
   ((struct test*)test3->ctx)->arg2 = 888;

   //任务放入线程池
   thread_task_post(tp, test1);
   thread_task_post(tp, test2);
   thread_task_post(tp, test3);

   sleep(10);
   thread_pool_destroy(tp);
   
}





