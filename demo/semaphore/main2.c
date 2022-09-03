#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)						   
#else
    union semun {
        int val;                             
        struct semid_ds *buf;    
        unsigned short int *array; 
        struct seminfo *__buf;  
    };
#endif     

//信号的初始化
static sem_initial(int semid){
    int ret;
    union semun semun;
    semun.val = 1;
    ret = semctl(semid,0,SETVAL,semun);
    if(ret == -1){
        fprintf(stderr, "semctl failed!\n");
    }
    return  ret;
}

//将p v操作封装成函数
//p操作
static int sem_p(int semid){
    int ret;
    struct sembuf sembuf;
    sembuf.sem_op = -1;//操作类型，设置为-1即p操作。
    sembuf.sem_num = 0;//指定信号量在信号量组中的编号
    sembuf.sem_flg = SEM_UNDO;//让操作系统跟踪信号，如果进程忘记释放，则操作系统进行负责释放。
    ret = semop(semid,&sembuf,1);//根据设置对信号量进行操作
    if (ret == -1) {
		fprintf(stderr, "sem_p failed!\n");
	}
    return ret;
}
//v操作
static int sem_v(int semid){
    int ret;
    struct sembuf sembuf;
    sembuf.sem_op = 1;//操作类型，设置为1即v操作。
    sembuf.sem_num = 0;//指定信号量在信号量组中的编号
    sembuf.sem_flg = SEM_UNDO;//让操作系统跟踪信号，如果进程忘记释放，则操作系统进行负责释放。
    ret = semop(semid,&sembuf,1);//根据设置对信号量进行操作
    if (ret == -1) {
		fprintf(stderr, "sem_v failed!\n");
	}
    return ret;
}

int main(int argc, char* argv[]) {
    int semid;

    //获取信号
    semid =semget((key_t)1234,1,0666 | IPC_CREAT);
    if(semid == -1){
        printf("semget failed!\n");
		exit(1);
    }
    //信号量的初始化
    if (argc > 1) {
		int ret = sem_initial(semid);
		if (ret == -1) {
			exit(1);
		}
	}
    for(;;){
        if(sem_p(semid) == -1){//p操作，申请，若无可用资源，则挂起等待。
            exit(1);
        }

        /* 模拟临界区----begin */
		printf("Process(%d) In\n", getpid());		
		sleep(3);
		printf("Process(%d) Out\n", getpid());
        /* 模拟临界区----end */ 
        if(sem_v(semid) == -1){//v操作，释放。
            exit(1);
        }
    }

	return 0;
}