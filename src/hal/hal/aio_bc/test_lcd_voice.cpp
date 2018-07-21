#include "net_tcp.h"
#include "common.h"
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
    
    unsigned char open_in[] = {0x96, 0x6b, 0x9d, 0x98, 0x76, 0x49, 0x50, 0x52, 0x54, 0x01};
    
    camera_ip = "192.168.1.101";//g_machine.channel_a.main_camera.device_ip_id;
    //      message = std::string((const char *)data, 399);
    message = std::string ((const char *)open_in, 10);
    
    std::string content = "{\"Page\":\"in_idle.html\",\"Key\":[{\"Name\":\"BCA_TEXT_REMAIN_SPACE\",\"Type\":0,\"Data\":\"0042\"},{\"Name\":\"BCA_TEXT_CHANNEL_NAME\",\"Type\":0,\"Data\":\"入口\"},{\"Name\":\"BCA_TEXT_CAR_IN_TIME\",\"Type\":0,\"Data\":\"2018-07-21,09:11\"},{\"Name\":\"BCA_TEXT_CAR_OUT_TIME\",\"Type\":0,\"Data\":\"2018-07-21,09:11\"},{\"Name\":\"BCA_TEXT_CONTENT0\",\"Type\":0,\"Data\":\"B\"},{\"Name\":\"BCA_TEXT_CONTENT1\",\"Type\":0,\"Data\":\"\"}]}0";
    
    std::string content_gbk;
    
    utf8togb2312(content, content_gbk);
    
    int l=content_gbk.length();
    
    cout << "l\t" << l << endl;
    
    unsigned short ls=(unsigned short)l;
    
    unsigned char l0=(unsigned char)((ls&0xff00)>>8);
    
    unsigned char l1=(unsigned char)(ls&0xff);
    
    printf("%x    %x    \n",  l0, l1);
    
    message += l0;
    
    message += l1;
    
    message += content_gbk;
    
    unsigned char cs = (unsigned char)0x30;
    
    message += cs;
    
    cout << "message length:\t" << message.length() << endl;
    
    cout << message << endl;
    
    for(int i=0;i<message.length();i++)
    {
        printf("%02x ", (unsigned char)message[i]);
    }
    cout << endl;
    
    
    if (tcp_client.connect_server(camera_ip, 5831))
    {
        printf("[AioCamera]连接相机%s的5831端口成功", camera_ip.c_str());
        b_connect = true;
    }
    else
    {
        sprintf(str_msg, "[AioCamera]连接相机%s的5831端口失败", camera_ip.c_str());
        b_connect = false;
    }
    if (b_connect)
    {
        
        std::string recv_msg;
        cout << message << endl;
        int n = tcp_client.send_data(message, recv_msg);
        tcp_client.dis_connect();
        cout << "recv message length:\t" << recv_msg.length() <<"\t"<< recv_msg << endl;
        if (recv_msg.length() > 0)    //有返回的消息
        {
            for(int i=0;i<recv_msg.length();i++)
            {
                printf("%02x ", (unsigned char)recv_msg[i]);
            }
        }
        cout << endl;
    }
}
