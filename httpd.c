/*
 * A simple webserver.
 * 常见结构体:
 * struct sockadd {
 *		unsigned short sa_family;
 *		char sa_data[14];
 * }
 *
 * 
 * struct sockaddr_in {					// sockadd的优化版
 *		short int sin_family;			// AF_INET
 *		unsigned short int sin_port;	// tcp/udp端口号 16位
 *		struct in_addr sin_addr;		// struct in_addr{unsigned long s_addr;}ip地址
 *		unsigned char sin_zero[8];
 * }
 */
#include <stdio.h>
#include <sys/socket.h>		// socket函数及数据结构
#include <sys/types.h>		// 数据类型定义
#include <netinet/in.h>		// 定义数据结构sockaddr_in
#include <arpa/inet.h>		// 提供IP地址转换函数
#include <unistd.h>			// 通用文件、目录、程序及进程操作函数
#include <ctype.h>			// 字符判断函数
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>		// 多线程操作函数
#include <sys/wait.h>		// 进程等待
#include <stdlib.h>
#include <stdint.h>

#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0/r/n"
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// 处理从套接字上监听到的一个HTTP请求
void accept_request(void *);

// 把错误信息写到perror并退出
void error_die(const char *sc);

// 读取套接字的一行, 把回车换行等情况都统一为换行符
int get_line(int, char *, int);

// 初始化httpd服务
int startup(u_short *port);

// 返回给浏览器表明受到的HTTP请求所用method不支持
void unimplemented(int);

// 找不到请求的文件
void not_found(int);

// 调用cat把文件返回给浏览器
void serve_file(int, const char *);

// 把HTTP响应头写入套接字
void headrs(int, const char *);

// 读取服务器上的文件写到套接字
void cat(int, FILE *);

// 运行cgi程序
void execute_cgi(int, const char *, const char *, const char *);

// 返回给客户端这是个错误请求
void bad_request(int);

// 处理cgi程序执行时出现的错误
void cannot_execute(int);


void accept_request(void *arg){
	int client = (intptr_t)arg;
	char buf[1024];
	size_t numchars;
	char method[255];		// 请求方式
	char url[255];
	char path[512];
	size_t i, j;
	struct stat st;
	int cgi = 0;

	char *query_string = NULL;

	numchars = get_line(client, buf, sizeof(buf));
	i = 0;
	j = 0;
	while(!ISspace(buf[i]) && (i < sizeof(method) - 1)){
		method[i] = buf[i];
		++i;
	}
	j = i;
	method[i] = '\0';

	// 忽略大小写
	if(strcasecmp(method, "GET") && strcasecmp(method, "POST")){
		unimplemented(client);
		return;
	}

	if(strcasecmp(method, "POST") == 0)
		cgi = 1;

	i = 0;
	while(ISspace(buf[j]) && (j < numchars))
		++j;
	while(!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars)){
		url[i] = buf[j];
		++i;
		++j;
	}
	url[i] = '\0';

	if(strcasecmp(method, "GET") == 0){
		query_string = url;
		while((*query_string != '?') && (*query_string != '\0'))
			++query_string;
		if(*query_string == '?'){
			cgi = 1;
			*query_string = '\0';
			++query_string;
		}
	}

	sprintf(path, "htdocs%s", url);
	if(path[strlen(path) - 1] == '/')
		strcat(path, "index.html");
	// 未找到cgi文件
	if(stat(path, &st) == -1){
		while((numchars > 0) && strcmp("\n", buf))
			numchars = get_line(client, buf, sizeof(buf));
		not_found(client);
	}
	else{
		// 文件是目录
		if((st.st_mode & S_IFMT) == S_IFDIR)
			strcat(path, "/index.html");
		// 文件可执行
		if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
			cgi = 1;
		// 不是cgi程序, 直接返回文件
		if(!cgi)
			serve_file(client, path);
		// 执行cgi程序
		else
			execute_cgi(client, path, method, query_string);
	}
	close(client);
}

void error_die(const char *sc){
	perror(sc);
	exit(1);
}

int get_line(int sock, char *buf, int size){
	int i = 0;
	char c = '\0';
	int n;

	while((i < size - 1) && (c != '\n')){
		n = recv(sock, &c, 1, 0);
		if(n > 0){
			if(c == '\r'){
				// MSG_PEEK从缓冲区获得一份副本, 不会移出缓冲区
				n = recv(sock, &c, 1, MSG_PEEK);
				if((n > 0)&&(c == '\n'))
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			++i;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';
	return i;
}

void not_found(int client){
	char buf[1024];
	
	sprintf(buf, "HTTP/1.1 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void unimplemented(int client){
	char buf[1024];
	sprintf(buf, "HTTP/1.1 501 Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</TITLE></HEAD>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

void headers(int client, const char *filename){
	char buf[1024];
	(void)filename;
	strcpy(buf, "HTTP/1.1 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, SERVER_STRING);
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}

void cat(int client, FILE *resource){
	char buf[1024];
	fgets(buf, sizeof(buf), resource);
	while(!feof(resource)){
		send(client, buf, strlen(buf), 0);
		fgets(buf, sizeof(buf), resource);
	}
}

void serve_file(int client, const char *filename){
	FILE *resource = NULL;
	int numchars = 1;
	char buf[1024];

	buf[0] = 'A';
	buf[1] = '\0';
	while((numchars > 0) && strcmp("\n", buf))
		numchars = get_line(client, buf, sizeof(buf));
	resource = fopen(filename, "r");
	if(resource == NULL)
		not_found(client);
	else{
		headers(client, filename);
		cat(client, resource);
	}
	fclose(resource);
}

void bad_request(int client){
	char buf[1024];

	sprintf(buf, "HTTP/1.1 400 BAD REQUEST\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "<P>Your browser sent a bad request, ");
	send(client, buf, sizeof(buf), 0);
	sprintf(buf, "such as a POST without a Content-Length.\r\n");
	send(client, buf, sizeof(buf), 0);
}

void cannot_execute(int client){
	char buf[1024];

	sprintf(buf, "HTTP/1.1 500 Internal Server Error\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
	send(client, buf, strlen(buf), 0);
}

void execute_cgi(int client, const char *path, const char *method, const char *query_string){
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	pid_t pid;
	int status;
	int i;
	char c;
	int numchars = 1;
	int content_length = -1;

	buf[0] = 'A';
	buf[1] = '\0';

	if(strcasecmp(method, "GET") == 0)
		while((numchars > 0) && strcmp("\n", buf))
			numchars = get_line(client, buf, sizeof(buf));
	else if(strcasecmp(method, "POST") == 0){
		numchars = get_line(client, buf, sizeof(buf));
		while((numchars > 0) && strcmp("\n", buf)){
			buf[15] = '\0';
			if(strcasecmp(buf, "Content-Length:") == 0)
				content_length = atoi(&(buf[16]));
			numchars = get_line(client, buf, sizeof(buf));
		}
		if(content_length == -1){
			bad_request(client);
			return;
		}
	}
	else{}

	// 创建管道, 与cgi程序通信
	if(pipe(cgi_output) < 0){
		cannot_execute(client);
		return;
	}
	if(pipe(cgi_input) < 0){
		cannot_execute(client);
		return;
	}
	// 创建子进程处理cgi程序
	if((pid = fork()) < 0){
		cannot_execute(client);
		return;
	}
	sprintf(buf, "HTTP/1.1 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	if(pid == 0){
		// 子进程
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		// 拷贝文件描述符
		dup2(cgi_output[1], STDOUT);
		dup2(cgi_input[0], STDIN);
		// 子进程关闭不使用的pipe描述符
		// 0: 读取 1: 输入
		close(cgi_output[0]);
		close(cgi_input[1]);
		sprintf(meth_env, "REQUEST_METHOD=%s", method);
		// 设置环境变量
		putenv(meth_env);
		if(strcasecmp(method, "GET") == 0){
			sprintf(query_env, "QUERY_STRING=%s", query_string);
			putenv(query_env);
		}
		else{
			sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
			putenv(length_env);
		}
		// 执行path指向的cgi程序
		execl(path, NULL);
		exit(0);
	}else{
		// 父进程: cgi_output 父进程读取 子进程输入
		//		   cgi_input 父进程输入 子进程读取
		close(cgi_output[1]);
		close(cgi_input[0]);
		if(strcasecmp(method, "POST") == 0)
			for(i = 0; i < content_length; ++i){
				recv(client, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		while(read(cgi_output[0], &c, 1) > 0)
			send(client, &c, 1, 0);

		close(cgi_output[0]);
		close(cgi_input[1]);
		// 等待子进程结束, 防止其成为僵尸进程
		waitpid(pid, &status, 0);
	}
}


int startup(u_short *port){
	int httpd = 0;
	int on = 1;
	struct sockaddr_in name;

	// 流套接字
	// 建立socket时是指定协议，应该用PF_xxxx
	httpd = socket(PF_INET, SOCK_STREAM, 0);
	if(httpd == -1)
		error_die("socket");
	// 初始化name
	memset(&name, 0, sizeof(name));
	// 设置地址族使用AF_xxxx ipv4
	name.sin_family = AF_INET;
	// 设置端口号, 使用htons(16位) 将主机字节顺序转化为网络字节顺序
	name.sin_port = htons(*port);
	// 设置ip地址, 使用htonl(32位)转化, ANY代表任意地址
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	/* tcp连接中 通过setsockopt设置一些机制: 超时、复用等
	 * arg1: 套接字描述符
	 * arg2: 被设置的选项level, SOL_SOCKET指定为套接字等级
	 * arg3: 打开或关闭地址复用功能
	 * arg4: 0 => 关闭 ， 非0 => 打开
	 * arg5: arg4长度
	 */
	if((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0){
		error_die("setsockopt failed");
	}
	// 绑定监听信息
	if(bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
		error_die("bind");
	if(*port == 0){
		// 动态生成了一个port
		socklen_t namelen = sizeof(name);
		// 获取一个与socket相关的地址
		if(getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
			error_die("getsockname");
		// 将网络字节顺序改为主机字节顺序
		*port = ntohs(name.sin_port);
	}
	// 开启监听
	if(listen(httpd, 5) < 0)
		error_die("listen");
	return httpd;
}


int main(void){
	int server_sock = -1;
	u_short port = 4000;		// 服务器端口号
	int client_sock = -1;
	struct sockaddr_in client_name;
	socklen_t client_name_len = sizeof(client_name);
	pthread_t newthread;

	server_sock = startup(&port);
	printf("httpd running on port %d\n", port);

	while(1){
		// 接受客户端请求
		client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
		if(client_sock == -1)
			error_die("accept");
		// 多线程处理请求
		// intptr_t来自头文件stdint.h, 位跨平台使用, 长度为所在平台位数, 用于存放地址
		if(pthread_create(&newthread, NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0)
			perror("pthread_create");
	}
	close(server_sock);
	return 0;
}

