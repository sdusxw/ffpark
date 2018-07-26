#include "net_tcp.h"
#include <iostream>

int main()
{
    NetTcpServer tcp_server_5232;
    tcp_server_5232.open_bind_listen(5232);
    while(true)
    {
        std::string msg;
        int length=0;
        if(tcp_server_5232.get_message(msg))
        {
            length = msg.length();
            printf("Got message with length:\t%d\n", length);
            std::cout << msg << std::endl;
            for(int i = 0;i<length;i++)
                printf("%02x ", (unsigned char)(msg[i]));
            std::cout << std::endl;
        }
 
        usleep(1000);
    }
}
