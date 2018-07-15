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
            printf("Got message with length:\t%d\n%s", length, msg);
        }
        sleep(1);
    }
}
