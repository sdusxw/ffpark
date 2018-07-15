#ifndef NET_UDP_H_
#define NET_UDP_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

//UDP发送类
class UdpSender
{
public:
	int port;	//UDP监听端口
	int sock;	//socket标识符
	bool is_open;
	struct sockaddr_in addr;
public:
	UdpSender(){port=0;sock=0;is_open=false;}
	bool open(char * str_dst_addr, int cli_port);
	bool send(std::string str_msg);
private:
	pthread_mutex_t mutex_lock;		//互斥锁
};
//UDP接收类
class UdpReceiver
{
public:
	int port;	//UDP监听端口
	int sock;	//socket标识符
	bool is_open;
	struct sockaddr_in addr;
public:
	UdpReceiver(){port=0;sock=0;is_open=false;}
	bool listen(int lis_port);
	std::string get_mesage();
private:
	pthread_mutex_t mutex_lock;		//互斥锁
};
//UDP发收类	先发送，然后等待回复
class NetUdp
{
public:
	int port;	//UDP监听端口
	int sock;	//socket标识符
	bool is_open;
	struct sockaddr_in addr;
public:
	NetUdp(){port=0;sock=0;is_open=false;}
	bool open(const char * str_dst_addr, int cli_port, int timeout_sec);
	bool sendrecv(std::string send_msg, std::string &recv_msg);
private:
	pthread_mutex_t mutex_lock;		//互斥锁
};
#endif
