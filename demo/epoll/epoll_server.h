#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<assert.h>
#include<fcntl.h>
#include<unistd.h>


typedef struct _ConnectStat ConnectStat; //因为下面的函数指针所以单独拿出来声明
typedef void(*response_handler) (ConnectStat * stat);//函数指针

// 该结构体用于保存链接状态
struct _ConnectStat {
	int fd;						//句柄
	char name[64];				//姓名
	char  age[64];				//年龄
	struct epoll_event _ev;		//当前文件句柄对应epoll事件
	int  status;				//0-未登录，1-已登录
	response_handler handler;	//处理函数
};

static int epfd = 0;

void usage(const char* argv);

void set_nonblock(int fd);

//初始化套接字
int startup(char* _ip, int _port);


//初始化ConnectStat
ConnectStat * stat_init(int fd);

// 初始化链接，然后等待浏览器发送请求
void connect_handle(int new_fd);


// http响应
void do_http_respone(ConnectStat * stat);

// 读取与解析http请求
void do_http_request(ConnectStat * stat);

// 刚进入页面的响应内容
void welcome_response_handler(ConnectStat * stat);

// 点击提交按钮后的响应内容
void commit_respone_handler(ConnectStat * stat);