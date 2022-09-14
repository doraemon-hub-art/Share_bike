#include "epoll_server.h"

const char *main_header = "HTTP/1.0 200 OK\r\nServer: Martin Server\r\nContent-Type: text/html\r\nConnection: Close\r\n";

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

	stat->_ev.events = EPOLLIN;// 读
	stat->_ev.data.ptr = stat;

	// 托管
	epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &stat->_ev);    
}


void do_http_respone(ConnectStat * stat) {
	stat->handler(stat);
}

// 读取http请求
void do_http_request(ConnectStat * stat) {

	char buf[4096];
	char * pos = NULL;
	ssize_t _s = read(stat->fd, buf, sizeof(buf) - 1);
	if (_s > 0){// 成功读取到数据
		buf[_s] = '\0';
		printf("receive from client:%s\n", buf);
		pos = buf;		
		if (!strncasecmp(pos, "GET", 3)) {// 请求页面
			stat->handler = welcome_response_handler;// 添加执行函数
		}else if (!strncasecmp(pos, "Post", 4)) {//发来数据
			
			printf("---Post----\n");
			pos += strlen("Post");
			while (*pos == ' ' || *pos == '/') ++pos;

			if (!strncasecmp(pos, "commit", 6)) {// 获取名字和年龄
				int len = 0;

				printf("post commit --------\n");
				pos = strstr(buf, "\r\n\r\n");
				char *end = NULL;
				if (end = strstr(pos, "name=")) {// 获取name
					pos = end + strlen("name=");
					end = pos;
					while (('a' <= *end && *end <= 'z') || ('A' <= *end && *end <= 'Z') || ('0' <= *end && *end <= '9'))	end++;
					len = end - pos;
					if (len > 0) {
						memcpy(stat->name, pos, end - pos);
						stat->name[len] = '\0';
					}
				}

				if (end = strstr(pos, "age=")) {// 获取age
					pos = end + strlen("age=");
					end = pos;
					while ('0' <= *end && *end <= '9')	end++;
					len = end - pos;
					if (len > 0) {
						memcpy(stat->age, pos, end - pos);
						stat->age[len] = '\0';
					}
				}
				stat->handler = commit_respone_handler;// 添加执行函数

			}
			else {
				stat->handler = welcome_response_handler;
			}

		}
		else {
			stat->handler = welcome_response_handler;
		}
	
		// 回复数据，设一个EPOLLOUT，监听写通道
		stat->_ev.events = EPOLLOUT;
		
		epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev); // 托管
	}else if (_s == 0)  {// 客户端关闭
		printf("client: %d close\n", stat->fd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, stat->fd, NULL);
		close(stat->fd);
		free(stat);
	}
	else{
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

	stat->_ev.events = EPOLLIN; // 修改，开始监听读事件。
	
	epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);


}

// 响应
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

	stat->_ev.events = EPOLLIN;// 读事件
	
	epoll_ctl(epfd, EPOLL_CTL_MOD, stat->fd, &stat->_ev);
}

