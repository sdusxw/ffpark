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
    
    unsigned char open_in[] = {0x96, 0x6b, 0x9d, 0x98, 0x76, 0x49, 0x50, 0x52, 0x54, 0x01};
    //unsigned char open_out[] ={0x14, 0x6b, 0x9d, 0x98, 0x40, 0x49, 0x50, 0x52, 0x54, 0x01, 0x00, 0x01, 0x02, 0x25};
    
    
        camera_ip = "192.168.1.103";//g_machine.channel_a.main_camera.device_ip_id;
        message = std::string((const char *)open_in, 10);
    
    std::string content = "{\"Page\": \"out_temp_car.html\",\"Key\": [{\"Name \": \"BCA_TEXT_CHARGE \",\"Type\": 0,\"Data\": \"10 元\"}, {\"Name \": \"BCA_TEXT_CAR_IN_TIME \",\"Type\": 0,\"Data\": \"2017 年 1 月 11 日 15: 00\"}, {\"Name \": \"BCA_TEXT_CAR_DURATION \",\"Type\": 0,\"Data\": \"00 天 01 小时 02 分\"}, {\"Name \": \"BCA_IMG_QRCODE \",\"Type\": 1,\"Data\": \"http: //abc.bluecardsoft.com.cn/service/Oauth2WebPay.aspx?Recor dID=12&PlateID=京 AB925E&Money=10.0&InTime=2017-1-11 15:00:00\"}],\"Voice\": \"收费金额为 10 元\"}";
    
    int l=content.length()+13;
    
    unsigned short ls=(unsigned short)l;
    
    unsigned char l0=(unsigned char)(ls&0xff00>>8);
    
    unsigned char l1=(unsigned char)(ls&0xff);
    
    message += l0;
    
    message += l1;
    
    message += content;
    
    unsigned char cs = (unsigned char)25;
    
    message += cs;
    
    cout << "message length:\t" << message.length() << endl;
    
    for(int i=0;i<message.length();i++)
    {
        printf("%2x ", message[i]);
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
                printf("%2x ", recv_msg[i]);
            }
        }
        cout << endl;
    }
}
