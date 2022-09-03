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

//�źŵĳ�ʼ��
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

//��p v������װ�ɺ���
//p����
static int sem_p(int semid){
    int ret;
    struct sembuf sembuf;
    sembuf.sem_op = -1;//�������ͣ�����Ϊ-1��p������
    sembuf.sem_num = 0;//ָ���ź������ź������еı��
    sembuf.sem_flg = SEM_UNDO;//�ò���ϵͳ�����źţ�������������ͷţ������ϵͳ���и����ͷš�
    ret = semop(semid,&sembuf,1);//�������ö��ź������в���
    if (ret == -1) {
		fprintf(stderr, "sem_p failed!\n");
	}
    return ret;
}
//v����
static int sem_v(int semid){
    int ret;
    struct sembuf sembuf;
    sembuf.sem_op = 1;//�������ͣ�����Ϊ1��v������
    sembuf.sem_num = 0;//ָ���ź������ź������еı��
    sembuf.sem_flg = SEM_UNDO;//�ò���ϵͳ�����źţ�������������ͷţ������ϵͳ���и����ͷš�
    ret = semop(semid,&sembuf,1);//�������ö��ź������в���
    if (ret == -1) {
		fprintf(stderr, "sem_v failed!\n");
	}
    return ret;
}

int main(int argc, char* argv[]) {
    int semid;

    //��ȡ�ź�
    semid =semget((key_t)1234,1,0666 | IPC_CREAT);
    if(semid == -1){
        printf("semget failed!\n");
		exit(1);
    }
    //�ź����ĳ�ʼ��
    if (argc > 1) {
		int ret = sem_initial(semid);
		if (ret == -1) {
			exit(1);
		}
	}
    for(;;){
        if(sem_p(semid) == -1){//p���������룬���޿�����Դ�������ȴ���
            exit(1);
        }

        /* ģ���ٽ���----begin */
		printf("Process(%d) In\n", getpid());		
		sleep(3);
		printf("Process(%d) Out\n", getpid());
        /* ģ���ٽ���----end */ 
        if(sem_v(semid) == -1){//v�������ͷš�
            exit(1);
        }
    }

	return 0;
}