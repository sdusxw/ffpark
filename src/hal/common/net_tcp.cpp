//利用Socket,网络中进程之间进行个通信
//http://blog.csdn.net/nana_93/article/details/8743525
#include "net_tcp.h"

#define DEBUG

NetTcpClient::NetTcpClient()
{
	connfd = -1;
}

NetTcpClient::~NetTcpClient()
{
	//close(connfd);
}

void NetTcpClient::dis_connect()
{
	close(connfd);
}

bool NetTcpClient::connect_server(std::string server_ip, int server_port)
{
	//!> 建立套接字
	if ((connfd = socket( AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("Tcp Client Socket Error %d ...\n", errno);
		return false;
	}

	//!> 套接字信息
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	inet_pton(AF_INET, server_ip.c_str(), &servaddr.sin_addr);

	//!> 链接server
	/*
	 * connect函数的第一个参数即为客户端的socket描述字，第二参数为服务器的socket地址，第三个参数为socket地址的长度。
	 * 客户端通过调用connect函数来建立与TCP服务器的连接。
	 */
	if (connect(connfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		printf("Tcp Client Connect error..\n");
		return false;
	}
	return true;
}

size_t NetTcpClient::send_data(std::string send_msg, std::string &recv_msg)
{
	size_t n_len = (size_t) (send_msg.length());
	size_t n_ret = write(connfd, send_msg.c_str(), n_len);
    printf("Sent:\t%ld\t%ld\n", n_len, n_ret);
	int n_recv = 0;
	char buf[2048];
    n_recv = receivedata(connfd, buf, 2048, 5000, NULL);
	recv_msg = std::string((const char *)buf, n_recv);
	return n_ret;
}

int receivedata(int socket, char * data, int length, int timeout,
		unsigned int * scope_id)
{

	int n;

	fd_set socketSet;
	struct timeval timeval1;
	FD_ZERO(&socketSet);
	FD_SET(socket, &socketSet);
	timeval1.tv_sec = timeout / 1000;
	timeval1.tv_usec = (timeout % 1000) * 1000;
	n = select(FD_SETSIZE, &socketSet, NULL, NULL, &timeval1);
	if (n < 0)
	{

		return -1;
	}
	else if (n == 0)
	{
		return 0;
	}

	n = recv(socket, data, length, 0);

	if (n < 0)
	{

	}

	return n;
}

void * getTcpStream(int s, int * size)
{
    char buf[2048];
    int n = 0;
    int i=0;
    while ((n = receivedata(s, buf, 2048, 5000, NULL)) > 0)
    {
        printf("%d\tTCP Stream\tcmd: %x, \tLength: %d\n", i, buf[0], n);
    }
    *size = 1;
    return (void*)buf;
}

void * getHTTPResponse(int s, int * size)
{
	char buf[2048];
	int n;
	int endofheaders = 0;
	int chunked = 0;
	int content_length = -1;
	unsigned int chunksize = 0;
	unsigned int bytestocopy = 0;
	/* buffers : */
	char * header_buf;
	unsigned int header_buf_len = 2048;
	unsigned int header_buf_used = 0;
	char * content_buf;
	unsigned int content_buf_len = 2048;
	unsigned int content_buf_used = 0;
	char chunksize_buf[32];
	unsigned int chunksize_buf_index;

	header_buf = (char *) malloc(header_buf_len);
	content_buf = (char *) malloc(content_buf_len);
	bzero(header_buf, header_buf_len);
	bzero(content_buf, content_buf_len);
	chunksize_buf[0] = '\0';
	chunksize_buf_index = 0;

	while ((n = receivedata(s, buf, 2048, 5000, NULL)) > 0)
	{
		if (endofheaders == 0)
		{
			int i;
			int linestart = 0;
			int colon = 0;
			int valuestart = 0;
			if (header_buf_used + n > header_buf_len)
			{
				header_buf = (char *) realloc(header_buf, header_buf_used + n);
				header_buf_len = header_buf_used + n;
			}
			memcpy(header_buf + header_buf_used, buf, n);
			header_buf_used += n;
			/* search for CR LF CR LF (end of headers)
			 * recognize also LF LF */
			i = 0;
			while (i < ((int) header_buf_used - 1) && (endofheaders == 0))
			{
				if (header_buf[i] == '\r')
				{
					i++;
					if (header_buf[i] == '\n')
					{
						i++;
						if (i < (int) header_buf_used && header_buf[i] == '\r')
						{
							i++;
							if (i < (int) header_buf_used
									&& header_buf[i] == '\n')
							{
								endofheaders = i + 1;
							}
						}
					}
				}
				else if (header_buf[i] == '\n')
				{
					i++;
					if (header_buf[i] == '\n')
					{
						endofheaders = i + 1;
					}
				}
				i++;
			}
			if (endofheaders == 0)
				continue;
			/* parse header lines */
			for (i = 0; i < endofheaders - 1; i++)
			{
				if (colon <= linestart && header_buf[i] == ':')
				{
					colon = i;
					while (i < (endofheaders - 1)
							&& (header_buf[i + 1] == ' '
									|| header_buf[i + 1] == '\t'))
						i++;
					valuestart = i + 1;
				}
				/* detecting end of line */
				else if (header_buf[i] == '\r' || header_buf[i] == '\n')
				{
					if (colon > linestart && valuestart > colon)
					{
#ifdef DEBUG
						printf("header='%.*s', value='%.*s'\n",
								colon - linestart, header_buf + linestart,
								i - valuestart, header_buf + valuestart);
#endif
						if (0
								== strncasecmp(header_buf + linestart,
										"content-length", colon - linestart))
						{
							content_length = atoi(header_buf + valuestart);
#ifdef DEBUG
							printf("Content-Length: %d\n", content_length);
#endif
						}
						else if (0
								== strncasecmp(header_buf + linestart,
										"transfer-encoding", colon - linestart)
								&& 0
										== strncasecmp(header_buf + valuestart,
												"chunked", 7))
						{
#ifdef DEBUG
							printf("chunked transfer-encoding!\n");
#endif
							chunked = 1;
						}
					}
					while ((i < (int) header_buf_used)
							&& (header_buf[i] == '\r' || header_buf[i] == '\n'))
						i++;
					linestart = i;
					colon = linestart;
					valuestart = 0;
				}
			}
			/* copy the remaining of the received data back to buf */
			n = header_buf_used - endofheaders;
			memcpy(buf, header_buf + endofheaders, n);
			/* if(headers) */
		}
		if (endofheaders)
		{
			/* content */
			if (chunked)
			{
				int i = 0;
				while (i < n)
				{
					if (chunksize == 0)
					{
						/* reading chunk size */
						if (chunksize_buf_index == 0)
						{
							/* skipping any leading CR LF */
							if (i < n && buf[i] == '\r')
								i++;
							if (i < n && buf[i] == '\n')
								i++;
						}
						while (i < n && isxdigit(buf[i])
								&& chunksize_buf_index
										< (sizeof(chunksize_buf) - 1))
						{
							chunksize_buf[chunksize_buf_index++] = buf[i];
							chunksize_buf[chunksize_buf_index] = '\0';
							i++;
						}
						while (i < n && buf[i] != '\r' && buf[i] != '\n')
							i++; /* discarding chunk-extension */
						if (i < n && buf[i] == '\r')
							i++;
						if (i < n && buf[i] == '\n')
						{
							unsigned int j;
							for (j = 0; j < chunksize_buf_index; j++)
							{
								if (chunksize_buf[j] >= '0'
										&& chunksize_buf[j] <= '9')
									chunksize = (chunksize << 4)
											+ (chunksize_buf[j] - '0');
								else
									chunksize = (chunksize << 4)
											+ ((chunksize_buf[j] | 32) - 'a'
													+ 10);
							}
							chunksize_buf[0] = '\0';
							chunksize_buf_index = 0;
							i++;
						}
						else
						{
							/* not finished to get chunksize */
							continue;
						}
#ifdef DEBUG
						printf("chunksize = %u (%x)\n", chunksize, chunksize);
#endif
						if (chunksize == 0)
						{
#ifdef DEBUG
							printf("end of HTTP content - %d %d\n", i, n);
							/*printf("'%.*s'\n", n-i, buf+i);*/
#endif
							goto end_of_stream;
						}
					}
					bytestocopy =
							((int) chunksize < (n - i)) ?
									chunksize : (unsigned int) (n - i);
					if ((content_buf_used + bytestocopy) > content_buf_len)
					{
						if (content_length
								>= (int) (content_buf_used + bytestocopy))
						{
							content_buf_len = content_length;
						}
						else
						{
							content_buf_len = content_buf_used + bytestocopy;
						}
						content_buf = (char *) realloc((void *) content_buf,
								content_buf_len);
					}
					memcpy(content_buf + content_buf_used, buf + i,
							bytestocopy);
					content_buf_used += bytestocopy;
					i += bytestocopy;
					chunksize -= bytestocopy;
				}
			}
			else
			{
				/* not chunked */
				if (content_length > 0
						&& (int) (content_buf_used + n) > content_length)
				{
					/* skipping additional bytes */
					n = content_length - content_buf_used;
				}
				if (content_buf_used + n > content_buf_len)
				{
					if (content_length >= (int) (content_buf_used + n))
					{
						content_buf_len = content_length;
					}
					else
					{
						content_buf_len = content_buf_used + n;
					}
					content_buf = (char *) realloc((void *) content_buf,
							content_buf_len);
				}
				memcpy(content_buf + content_buf_used, buf, n);
				content_buf_used += n;
			}
		}
		/* use the Content-Length header value if available */
		if (content_length > 0 && (int) content_buf_used >= content_length)
		{
#ifdef DEBUG
			printf("End of HTTP content\n");
#endif
			break;
		}
	}
	end_of_stream: free(header_buf);
	header_buf = NULL;
	*size = content_buf_used;
	if (content_buf_used == 0)
	{
		free(content_buf);
		content_buf = NULL;
	}
	return content_buf;
}

//socket是“open—write/read—close”模式的一种实现
bool NetHttpServer::open_bind_listen(int listen_port)
{
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{                           //调用socket创建用于监听客户端的socket
		perror("Creating socket failed.");
		exit(1);
	}

	int opt = SO_REUSEADDR;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //设置socket属性

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(listen_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenfd, (struct sockaddr *) &server, sizeof(struct sockaddr))
			== -1)
	{                           //调用bind绑定地址
		perror("Bind error.");
		exit(1);
	}

	if (listen(listenfd, BACKLOG) == -1)
	{                           //调用listen开始监听
		perror("listen() error\n");
		exit(1);
	}

	//初始化select
	maxfd = listenfd;
	maxi = -1;
	for (i = 0; i < FD_SETSIZE; i++)
	{
		client[i].fd = -1;
	}
	FD_ZERO(&allset);           //清空
	FD_SET(listenfd, &allset);  //将监听socket加入select检测的描述符集合
	return true;
}

bool NetHttpServer::get_message(std::string &msg)
{
	struct sockaddr_in addr;
	rset = allset;
	nready = select(maxfd + 1, &rset, NULL, NULL, NULL);    //调用select
	printf("Select() break and the return num is %d. \n", nready);

	if (FD_ISSET(listenfd, &rset))
	{                       //检测是否有新客户端请求
		printf("Accept a connection.\n");
		//调用accept，返回服务器与客户端连接的socket描述符
		sin_size = sizeof(struct sockaddr_in);
		if ((connectfd = accept(listenfd, (struct sockaddr *) &addr,
				(socklen_t *) &sin_size)) == -1)
		{
			perror("Accept() error\n");
			exit(-1);
		}

		//将新客户端的加入数组
		for (i = 0; i < FD_SETSIZE; i++)
		{
			if (client[i].fd < 0)
			{
				char buffer[20];
				client[i].fd = connectfd;   //保存客户端描述符
				memset(buffer, '0', sizeof(buffer));
				sprintf(buffer, "Client[%.2d]", i);
				memcpy(client[i].name, buffer, strlen(buffer));
				client[i].addr = addr;
				memset(buffer, '0', sizeof(buffer));
				sprintf(buffer, "Only For Test!");
				memcpy(client[i].data, buffer, strlen(buffer));
				printf("You got a connection from %s:%d.\n",
						inet_ntoa(client[i].addr.sin_addr),
						ntohs(client[i].addr.sin_port));
				printf("Add a new connection:%s\n", client[i].name);
				break;
			}
		}

		if (i == FD_SETSIZE)
			printf("Too many clients\n");
		//FD_SET(connectfd, &allset); //将新socket连接放入select监听集合
		if (connectfd > maxfd)
			maxfd = connectfd;  //确认maxfd是最大描述符
		if (i > maxi)       //数组最大元素值
			maxi = i;
		//if (--nready <= 0)
		//continue;       //如果没有新客户端连接，继续循环
	}

	for (i = 0; i <= maxi; i++)
	{
		printf("Start Recv:\t%d\n", i);
		if ((sockfd = client[i].fd) < 0)    //如果客户端描述符小于0，则没有客户端连接，检测下一个
			continue;
		// 有客户连接，检测是否有数据
		int n_http = 0;
		char *p_http_response = (char *) getHTTPResponse(sockfd, &n_http);
        if(n_http>0)
            msg = p_http_response;
		if(p_http_response)
			free(p_http_response);
		close(sockfd);
		client[i].fd = -1;
		break;
	}
	return true;
}

bool NetTcpServer::open_bind_listen(int listen_port)
{
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {                           //调用socket创建用于监听客户端的socket
        perror("Creating socket failed.");
        exit(1);
    }
    
    int opt = SO_REUSEADDR;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //设置socket属性
    
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(listen_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(listenfd, (struct sockaddr *) &server, sizeof(struct sockaddr))
        == -1)
    {                           //调用bind绑定地址
        perror("Bind error.");
        exit(1);
    }
    
    if (listen(listenfd, BACKLOG) == -1)
    {                           //调用listen开始监听
        perror("listen() error\n");
        exit(1);
    }
    
    //初始化select
    maxfd = listenfd;
    maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++)
    {
        client[i].fd = -1;
    }
    FD_ZERO(&allset);           //清空
    FD_SET(listenfd, &allset);  //将监听socket加入select检测的描述符集合
    return true;
}

bool NetTcpServer::get_message(std::string &msg)
{
    struct sockaddr_in addr;
    rset = allset;
    nready = select(maxfd + 1, &rset, NULL, NULL, NULL);    //调用select
    printf("Select() break and the return num is %d. \n", nready);
    
    if (FD_ISSET(listenfd, &rset))
    {                       //检测是否有新客户端请求
        printf("Accept a connection.\n");
        //调用accept，返回服务器与客户端连接的socket描述符
        sin_size = sizeof(struct sockaddr_in);
        if ((connectfd = accept(listenfd, (struct sockaddr *) &addr,
                                (socklen_t *) &sin_size)) == -1)
        {
            perror("Accept() error\n");
            exit(-1);
        }
        
        //将新客户端的加入数组
        for (i = 0; i < FD_SETSIZE; i++)
        {
            if (client[i].fd < 0)
            {
                char buffer[20];
                client[i].fd = connectfd;   //保存客户端描述符
                memset(buffer, '0', sizeof(buffer));
                sprintf(buffer, "Client[%.2d]", i);
                memcpy(client[i].name, buffer, strlen(buffer));
                client[i].addr = addr;
                memset(buffer, '0', sizeof(buffer));
                sprintf(buffer, "Only For Test!");
                memcpy(client[i].data, buffer, strlen(buffer));
                printf("You got a connection from %s:%d.\n",
                       inet_ntoa(client[i].addr.sin_addr),
                       ntohs(client[i].addr.sin_port));
                printf("Add a new connection:%s\n", client[i].name);
                break;
            }
        }
        
        if (i == FD_SETSIZE)
            printf("Too many clients\n");
        //FD_SET(connectfd, &allset); //将新socket连接放入select监听集合
        if (connectfd > maxfd)
            maxfd = connectfd;  //确认maxfd是最大描述符
        if (i > maxi)       //数组最大元素值
            maxi = i;
        //if (--nready <= 0)
        //continue;       //如果没有新客户端连接，继续循环
    }
    
    for (i = 0; i <= maxi; i++)
    {
        printf("Start Recv:\t%d\n", i);
        if ((sockfd = client[i].fd) < 0)    //如果客户端描述符小于0，则没有客户端连接，检测下一个
            continue;
        // 有客户连接，检测是否有数据
        int n_http = 0;
        char *p_http_response = (char *) getTcpStream(sockfd, &n_http);
        if(n_http>0)
            msg = "A";//p_http_response;
        /*if(p_http_response)
            free(p_http_response);*/
        close(sockfd);
        client[i].fd = -1;
        break;
    }
    return true;
}

