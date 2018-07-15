#include "net_tcp.h"
#include <iostream>

int main()
{
    NetHttpServer http_server8090;
    http_server8090.open_bind_listen(8090);
    while(true)
    {
        std::string push_msg;
        if(http_server8090.get_message(push_msg))
        {
            std::cout << push_msg << std::endl;
        }
        sleep(1);
    }
}
