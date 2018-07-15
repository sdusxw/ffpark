#include "net_tcp.h"
#include <iostream>

int main()
{
    NetHttpServer http_server8090;
    http_server8090.open_bind_listen(8090);
    while(true)
    {
        char * msg = NULL;
        int length;
        if(http_server8090.get_message(msg, length))
        {
            printf("Got message with length:\t%d\n", length);
            FILE *fid;
            fid = fopen("binary.dat","wb");
            if(fid == NULL)
            {
                printf("写出文件出错");
            }
            
            fwrite(msg, length, 1, fid);
            
            fclose(fid);
        }
        sleep(1);
    }
}
