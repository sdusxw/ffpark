#ifndef NET_TCP_H_
#define NET_TCP_H_

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <sys/poll.h>
#include <sys/time.h>

#define BUF_LEN 1024   //缓冲区大小
#define SERV_PORT 8080 //通讯端口
#define FD_SIZE 100    //FD_SIZE为select函数支持的最大描述符个数
#define MAX_BACK 100   //listen队列中等待的连接数

#define BACKLOG 10               //listen队列中等待的连接数
#define MAXDATASIZE 4096        //缓冲区大小

typedef struct _CLIENT
{
	int fd;                     //客户端socket描述符
	char name[20];              //客户端名称
	struct sockaddr_in addr;    //客户端地址信息结构体
	char data[MAXDATASIZE];     //客户端私有数据指针
} CLIENT;

int receivedata(int socket, char * data, int length, int timeout,
		unsigned int * scope_id);

void * getHTTPResponse(int s, int * size);

class NetTcpClient
{
public:
	NetTcpClient();
	~NetTcpClient();

	bool connect_server(std::string server_ip, int server_port);
	size_t send_data(std::string send_msg, std::string &recv_msg);
	void dis_connect();
public:
	int connfd;
	struct sockaddr_in servaddr;     //服务器地址信息结构体
};

class NetHttpServer
{
public:
	bool open_bind_listen(int listen_port);
	bool get_message(char * ptr_msg, int &len_msg);

public:
	int i, maxi, maxfd, sockfd;
	int nready;
	ssize_t n;
	fd_set rset, allset;        //select所需的文件描述符集合
	int listenfd, connectfd;    //socket文件描述符
	struct sockaddr_in server;  //服务器地址信息结构体

	CLIENT client[FD_SETSIZE];  //FD_SETSIZE为select函数支持的最大描述符个数
	char recvbuf[MAXDATASIZE];  //缓冲区
	int sin_size;               //地址信息结构体大小
};

class NetTcpServer
{
public:
    bool open_bind_listen(int listen_port);
    bool get_message(unsigned char* message, unsigned int length);
    
public:
    int i, maxi, maxfd, sockfd;
    int nready;
    ssize_t n;
    fd_set rset, allset;        //select所需的文件描述符集合
    int listenfd, connectfd;    //socket文件描述符
    struct sockaddr_in server;  //服务器地址信息结构体
    
    CLIENT client[FD_SETSIZE];  //FD_SETSIZE为select函数支持的最大描述符个数
    char recvbuf[MAXDATASIZE];  //缓冲区
    int sin_size;               //地址信息结构体大小
};

#endif
