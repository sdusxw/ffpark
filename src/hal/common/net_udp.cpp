#include "net_udp.h"

bool UdpSender::open(char * str_dst_addr, int cli_port)
{
	port = cli_port;
	pthread_mutex_init(&mutex_lock, NULL);
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket client error!");
		return false;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(str_dst_addr);
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		printf("Incorrect ip address!");
		close(sock);
		return false;
	}
	is_open = true;
	return true;
}

bool UdpSender::send(std::string str_msg)
{
	if (!is_open)
		return false;
	pthread_mutex_lock(&mutex_lock);
	size_t n_send = sendto(sock, str_msg.c_str(), strlen(str_msg.c_str()),
			0, (struct sockaddr *) &addr, sizeof(addr));
	pthread_mutex_unlock(&mutex_lock);
	if (-1 == n_send)
		return false;
	else
		return true;
}

bool UdpReceiver::listen(int lis_port)
{
	port = lis_port;
	pthread_mutex_init(&mutex_lock, NULL);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket client error!");
		return false;
	}
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return false;
	}
	is_open = true;
	return true;
}

std::string UdpReceiver::get_mesage()
{
	std::string msg = "";
	if (is_open)
	{
		int addr_len = sizeof(addr);
		char buff[4096] =
		{ 0 };
		pthread_mutex_lock(&mutex_lock);
		size_t n_recv = recvfrom(sock, buff, 4096, 0,
				(struct sockaddr *) &addr,
				(socklen_t*) &addr_len);
		pthread_mutex_unlock(&mutex_lock);
		if (n_recv > 0)
			msg = buff;
	}
	return msg;
}

bool NetUdp::open(const char * str_dst_addr, int cli_port, int timeout_sec)
{
	port = cli_port;
	pthread_mutex_init(&mutex_lock, NULL);
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("socket client error!");
		return false;
	}
	struct timeval tv_out;
	tv_out.tv_sec = timeout_sec;
	tv_out.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(str_dst_addr);
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		printf("Incorrect ip address!");
		close(sock);
		return false;
	}
	is_open = true;
	return true;
}

bool NetUdp::sendrecv(std::string send_msg, std::string &recv_msg)
{
	bool result = false;
	pthread_mutex_lock(&mutex_lock);
	if (!is_open)
	{
		result = false;
	}
	else
	{
		//发送消息
		size_t n_send = sendto(sock, send_msg.c_str(),
				strlen(send_msg.c_str()), 0,
				(struct sockaddr *) &addr, sizeof(addr));
		if (-1 == n_send)
		{
			//发送失败则返回
			result = false;
		}
		else
		{
			//阻塞式等待接收消息，超时自动返回
			int addr_len = sizeof(addr);
			char buff[4096] =
			{ 0 };
			size_t n_recv = recvfrom(sock, buff, 4096, 0,
					(struct sockaddr *) &addr,
					(socklen_t*) &addr_len);
			if (n_recv > 0)
			{
				recv_msg = buff;
				result = true;
			}
			else
			{
				result = false;
			}
		}
	}
	pthread_mutex_unlock(&mutex_lock);
	return result;
}
