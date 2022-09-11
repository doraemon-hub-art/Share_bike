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


typedef struct _ConnectStat  ConnectStat;

typedef void(*response_handler) (ConnectStat * stat);//函数指针

struct _ConnectStat {
	int fd;						//句柄
	char name[64];				//姓名
	char  age[64];				//年龄
	struct epoll_event _ev;		//当前文件句柄对应epoll事件
	int  status;				//0-未登录，1-已登录
	response_handler handler;	//处理函数
};


ConnectStat * stat_init(int fd);
void connect_handle(int new_fd);
void do_http_respone(ConnectStat * stat);
void do_http_request(ConnectStat * stat);
void welcome_response_handler(ConnectStat * stat);
void commit_respone_handler(ConnectStat * stat);

const char *main_header = "HTTP/1.0 200 OK\r\nServer: Martin Server\r\nContent-Type: text/html\r\nConnection: Close\r\n";

static int epfd = 0;

void usage(const char* argv)
{
	printf("%s:[ip][port]\n", argv);
}

void set_nonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

int startup(char* _ip, int _port)  
{
	
	int sock = socket(AF_INET, SOCK_STREAM, 0);//创建套接字
	if (sock < 0)
	{
		perror("sock");
		exit(2);
	}

	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	
	struct sockaddr_in local;
	local.sin_port = htons(_port);
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(_ip);

	
	if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)//绑定
	{
		perror("bind");
		exit(3);
	}
	
	if (listen(sock, 5) < 0)//监听
	{
		perror("listen");
		exit(4);
	}
	
	return sock;//返回一个server SOCKET 文件句柄
}


int main(int argc, char *argv[]){

	if (argc != 3)//检查输入的参数个数是否正确
	{
		usage(argv[0]);
		exit(1);
	}
	
	//创建一个server socket 句柄
	int listen_sock = startup(argv[1], atoi(argv[2]));      

	//创建epoll
	epfd = epoll_create(256);//设置可处理的最大句柄数为256,其实实际上并没有限制，感觉这个接口有点被弃用了。 
	if (epfd < 0)//创建失败
	{
		perror("epoll_create");
		exit(5);
	}

	struct epoll_event _ev;//
	ConnectStat * stat = stat_init(listen_sock);
	_ev.events = EPOLLIN;    //设置关心事件为读事件     
	_ev.data.ptr = stat;
	

	
	//托管
	epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &_ev);//将listen_sock添加到epfd中，关心读事件

	struct epoll_event revs[64];

	int timeout = -1;
	int num = 0;
	int done = 0;

	while (!done){
		
		switch ((num = epoll_wait(epfd, revs, 64, timeout))){
		case 0:                  
			printf("timeout\n");
			break;
		case -1:                 
			perror("epoll_wait");
			break;
		default:{
			struct sockaddr_in peer;
			socklen_t len = sizeof(peer);

			int i;
			for (i = 0; i < num; i++){
				ConnectStat * stat = (ConnectStat *)revs[i].data.ptr;

				int rsock = stat->fd; 
				if (rsock == listen_sock && (revs[i].events) && EPOLLIN) 
				{
					int new_fd = accept(listen_sock, (struct sockaddr*)&peer, &len);

					if (new_fd > 0){
						printf("get a new client:%s:%d\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));
						
						connect_handle(new_fd);
					}
				}
				else 

				{
					if (revs[i].events & EPOLLIN)
					{
						do_http_request((ConnectStat *)revs[i].data.ptr);
					}
					else if (revs[i].events & EPOLLOUT)
					{
						do_http_respone((ConnectStat *)revs[i].data.ptr);
					}
					else
					{
					}
				}
			}
		}
		break;
		}
	}
	return 0;
}


//初始化ConnectStat
ConnectStat * stat_init(int fd) {
	ConnectStat * temp = NULL;
	temp = (ConnectStat *)malloc(sizeof(ConnectStat));

	if (!temp) {
		fprintf(stderr, "malloc failed. reason: %m\n");
		return NULL;
	}

	memset(temp, '\0', sizeof(ConnectStat));
	temp->fd = fd;
	temp->status = 0;
	

}


void connect_handle(int new_fd) {
	ConnectStat *stat = stat_init(new_fd);
	set_nonblock(new_fd);

	stat->_ev.events = EPOLLIN;
	stat->_ev.data.ptr = stat;

	epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &stat->_ev);    

}

void do_http_respone(ConnectStat * stat) {
	stat->handler(stat);
}

void do_http_request(ConnectStat * stat) {

	
	char buf[4096];
	char * pos = NULL;
	
	ssize_t _s = read(stat->fd, buf, sizeof(buf) - 1);
	if (_s > 0)
	{
		buf[_s] = '\0';
		printf("receive from client:%s\n", buf);

		pos = buf;

		
		if (!strncasecmp(pos, "GET", 3)) {
			stat->handler = welcome_response_handler;
		}
		else if (!strncasecmp(pos, "Post", 4)) {
			
			printf("---Post----\n");
			pos += strlen("Post");
			while (*pos == ' ' || *pos == '/') ++pos;

			if (!strncasecmp(pos, "commit", 6)) {
				int len = 0;

				printf("post commit --------\n");
				pos = strstr(buf, "\r\n\r\n");
				char *end = NULL;
				if (end = strstr(pos, "name=")) {
					pos = end + strlen("name=");
					end = pos;
					while (('a' <= *end && *end <= 'z') || ('A' <= *end && *end <= 'Z') || ('0' <= *end && *end <= '9'))	end++;
					len = end - pos;
					if (len > 0) {
						memcpy(stat->name, pos, end - pos);
						stat->name[len] = '\0';
					}
				}

				if (end = strstr(pos, "age=")) {
					pos = end + strlen("age=");
					end = pos;
					while ('0' <= *end && *end <= '9')	end++;
					len = end - pos;
					if (len > 0) {
						memcpy(stat->age, pos, end - pos);
						stat->age[len] = '\0';
					}
				}
				stat->handler = commit_respone_handler;

			}
			else {
				stat->handler = welcome_response_handler;
			}

		}
		else {
			stat->handler = welcome_response_handler;
		}

		

		stat->_ev.events = EPOLLOUT;
		
		epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);    
	}
	else if (_s == 0)  
	{
		printf("client: %d close\n", stat->fd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, stat->fd, NULL);
		close(stat->fd);
		free(stat);
	}
	else
	{
		perror("read");
	}

}


void welcome_response_handler(ConnectStat * stat) {
	const char * welcome_content = "\
<html lang=\"zh-CN\">\n\
<head>\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
<title>This is a test</title>\n\
</head>\n\
<body>\n\
<div align=center height=\"500px\" >\n\
<br/><br/><br/>\n\
<h2>我是标题</h2><br/><br/>\n\
<form action=\"commit\" method=\"post\">\n\
姓名: <input type=\"text\" name=\"name\" />\n\
<br/>年龄: <input type=\"password\" name=\"age\" />\n\
<br/><br/><br/><input type=\"submit\" value=\"提交\" />\n\
<input type=\"reset\" value=\"重置\" />\n\
</form>\n\
</div>\n\
</body>\n\
</html>";

	char sendbuffer[4096];
	char content_len[64];

	strcpy(sendbuffer, main_header);
	snprintf(content_len, 64, "Content-Length: %d\r\n\r\n", (int)strlen(welcome_content));
	strcat(sendbuffer, content_len);
	strcat(sendbuffer, welcome_content);
	printf("send reply to client \n%s", sendbuffer);

	write(stat->fd, sendbuffer, strlen(sendbuffer));

	stat->_ev.events = EPOLLIN;
	
	epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);


}


void commit_respone_handler(ConnectStat * stat) {
	const char * commit_content = "\
<html lang=\"zh-CN\">\n\
<head>\n\
<meta content=\"text/html; charset=utf-8\" http-equiv=\"Content-Type\">\n\
<title>This is a test</title>\n\
</head>\n\
<body>\n\
<div align=center height=\"500px\" >\n\
<br/><br/><br/>\n\
<h2>欢迎&nbsp;%s &nbsp;,你的年龄是&nbsp;%s！</h2><br/><br/>\n\
</div>\n\
</body>\n\
</html>\n";

	char sendbuffer[4096];
	char content[4096];
	char content_len[64];
	int len = 0;

	len = snprintf(content, 4096, commit_content, stat->name, stat->age);
	strcpy(sendbuffer, main_header);
	snprintf(content_len, 64, "Content-Length: %d\r\n\r\n", len);
	strcat(sendbuffer, content_len);
	strcat(sendbuffer, content);
	printf("send reply to client \n%s", sendbuffer);

	write(stat->fd, sendbuffer, strlen(sendbuffer));

	stat->_ev.events = EPOLLIN;
	
	epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);
}