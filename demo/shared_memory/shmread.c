#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>

struct Conn_stat{
	int count;
	char ip[64];
	
};

int main(void){

    void *shm = NULL;//����Ĺ����ڴ��ԭʼ�׵�ַ
    struct Conn_stat *stat = NULL;
    int shmid;//�����ڴ��ʶ��
    //���������ڴ�
    shmid = shmget((key_t)1234, sizeof(struct Conn_stat), 0666|IPC_CREAT);
    if(shmid == -1){
		fprintf(stderr, "shmget failed\n");
		exit(0);
	}
    //�������ڴ�ҽӵ���ǰ��ǰ���̵ĵ�ַ�ռ�
    shm = shmat(shmid, 0, 0);
    if(shm == (void*)-1){
		fprintf(stderr, "shmat failed\n");
		exit(1);
	}
    printf("\nMemory attached at %p\n", shm);
    //���ù����ڴ�
    stat = (struct Conn_stat*)shm;
    int i = 0;
	while((i++) < 10){
		printf("ip = %s ,count: %d\t\t\n", stat->ip, stat->count);	
		sleep(1);
	}
    //�ѹ����ڴ�ӵ�ǰ�����з���
    if(shmdt(shm) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(2);
	}
    //ɾ�������ڴ�
    if(shmctl(shmid, IPC_RMID, 0) == -1){
		fprintf(stderr, "shmctl(IPC_RMID) failed, reason: %s\n",strerror(errno));
		exit(3);
	}
    return 0;   
}