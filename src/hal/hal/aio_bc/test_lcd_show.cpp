#include "net_tcp.h"
#include "common.h"
#include <jsoncpp/json/json.h>
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
    
    camera_ip = "192.168.1.103";//g_machine.channel_a.main_camera.device_ip_id;
    //      message = std::string((const char *)data, 399);
    message = std::string ((const char *)open_in, 10);
    
    Json::Value json_lcd_show;
    Json::Value json_key, json_keys;
    json_lcd_show["Page"] = "out_temp_car.html";
    json_key["Name"] = Json::Value("BCA_IMG_QRCODE");
    json_key["Type"] = 0;
    json_key["Data"] = "http://ipark.sdboon.com/slyy/out.php?park_id=0531000008&box_ip=192.168.1.201";
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_PLATE";
    json_key["Type"] = 0;
    json_key["Data"] = "鲁AB925E";
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CHARGE";
    json_key["Type"] = 0;
    json_key["Data"] = "16元";
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CAR_IN_TIME";
    json_key["Type"] = 0;
    json_key["Data"] = "2018-07-23,10:21";
    json_keys.append(json_key);
    
    json_key["Name"] = "BCA_TEXT_CAR_DURATION";
    json_key["Type"] = 0;
    json_key["Data"] = "8小时18分钟";
    json_keys.append(json_key);
    
    json_lcd_show["Key"] = json_keys;
    
    cout << "Json UTF8:\t" << json_lcd_show.toStyledString() << endl;
    
    std::string content_gbk;
    
    utf8togb2312(json_lcd_show.toStyledString(), content_gbk);
    
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
