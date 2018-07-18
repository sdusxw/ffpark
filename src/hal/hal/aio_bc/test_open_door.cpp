#include "net_tcp.h"
#include <iostream>

using namespace std;

int main()
{
    bool b_connect = false;
    NetTcpClient tcp_client;
    char str_msg[1024];
    std::string log_msg;
    std::string camera_ip = "";
    std::string message;
    
    unsigned char open_in[] = {0x14, 0x6b, 0x9d, 0x98, 0x40, 0x49, 0x50, 0x52, 0x54, 0x01, 0x00, 0x01, 0x02, 0x25};
    //unsigned char open_out[] ={0x14, 0x6b, 0x9d, 0x98, 0x40, 0x49, 0x50, 0x52, 0x54, 0x01, 0x00, 0x01, 0x02, 0x25};
    
    
        camera_ip = "192.168.1.101";//g_machine.channel_a.main_camera.device_ip_id;
        message = std::string((const char *)open_in, 14);
    
    if (tcp_client.connect_server(camera_ip, 5231))
    {
        printf("[AioCamera]连接相机%s的5231端口成功", camera_ip.c_str());
        b_connect = true;
    }
    else
    {
        sprintf(str_msg, "[AioCamera]连接相机%s的5231端口失败", camera_ip.c_str());
        b_connect = false;
    }
    if (b_connect)
    {
        
        std::string recv_msg;
        cout << message << endl;
        int n = tcp_client.send_data(message, recv_msg);
        tcp_client.dis_connect();
        std::cout << recv_msg << std::endl;
        if (recv_msg.length() > 0)    //有返回的消息
        {
            
        }
    }
}
