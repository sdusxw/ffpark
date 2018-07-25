#include "net_tcp.h"
#include <iostream>

int main()
{
    NetTcpServer tcp_server_5232;
    tcp_server_5232.open_bind_listen(5232);
    while(true)
    {
        char * msg = NULL;
        int length;
        if(tcp_server_5232.get_message(msg, length))
        {
            printf("Got message with length:\t%d\n", length);
            char filename[128];
            sprintf(filename, "tcp_%d.bin", length);
            FILE *fid;
            fid = fopen(filename,"wb");
            if(fid == NULL)
            {
                printf("写出文件出错");
            }
            
            fwrite(msg, length, 1, fid);
            
            fclose(fid);
        }
 
        usleep(1000);
    }
}
