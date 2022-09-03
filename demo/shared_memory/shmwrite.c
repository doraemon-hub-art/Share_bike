#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>

struct Conn_stat{
	int count;
	char ip[64];
};

int main(void){

    void *shm = NULL;
    int shmid = 0;//�洢�����ڴ�ı�ʶ
    struct Conn_stat stat = {0,"127.0.0.1"};

    //���������ڴ�
    shmid = shmget((key_t)1234,sizeof(struct Conn_stat),0666 | IPC_CREAT);
    if(shmid == -1){
        fprintf(stderr, "shmget failed\n");
		exit(1);
    }
    //�������ڴ�ҽӵ���ǰ���̵ĵ�ַ�ռ�
    shm = shmat(shmid,(void*)0,0);
    if(shm == (void*)-1){
		fprintf(stderr, "shmat failed\n");
		exit(2);
	}
    printf("Memory attached at %p\n", shm);
    //���ù����ڴ�
    struct Conn_stat *p = (struct Conn_stat*)shm;
    memcpy(p,&stat,sizeof(struct Conn_stat));
    //�޸Ĺ����ڴ��е�����
    int i = 0;
    while((i++) < 50){
        p->count++;
        sleep(1);
    }
    //�ѹ����ڴ�ӵ�ǰ�����з���
    if(shmdt(shm) == -1){
		fprintf(stderr, "shmdt failed\n");
		exit(3);
	}
    return 0;
}