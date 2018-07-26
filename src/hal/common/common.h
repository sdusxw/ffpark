#ifndef _BOON_COMMON_H
#define _BOON_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <sys/types.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <jsoncpp/json/json.h>
#include <vector>
#include <time.h>

//全局日志文件变量
extern std::fstream g_log_file;
//控制台消息输出：输出格式为时间+消息
void msg_print(std::string msg);
//日志输出初始化，生成日志文件，追加模式
void log_init();
//日志输出：输出格式为时间+日志消息
void log_output(std::string msg);
//获取时间，精确到微秒
std::string get_time_us();
//获取时间，精确到毫秒
std::string get_time_ms();
//获取时间，精确到妙
std::string get_time_sec();
//获取日期，精确到天
std::string get_time_date();
//获取Unix时间戳
long get_unix_ts();
//获取ip地址
std::string get_local_ip();
//获取ip地址，需要输入网卡设备名称
bool get_local_ip(std::string dev, std::string & str_ip);
//创建文件夹
int create_dir(const char *s_path_name);
//进程互斥
bool is_have_instance();
//JPG解压函数
bool jpeg_uncompress(const char * jpeg_data, int jpeg_size, char *rgb_data,
		int rgb_size, int w, int h, int c);
//string 字符串分割
std::vector<std::string> string_split(std::string str, std::string pattern);
//设备类
class Device
{
public:
	std::string device_type;		//设备类型（中维抓拍相机、中维智能相机、网络继电器）
	std::string device_ip_id;			//设备IP地址
	std::string device_username;	//设备用户名
	std::string device_password;	//设备密码
};
//通道类
class Channel
{
public:
	std::string channel_id;	//	通道id
	std::string in_out;	//入口|出口
	Device main_camera;	//主摄像机
	Device aux_camera;		//辅摄像机
	Device net_relay;			//网络继电器
	std::vector<Device> devices; //设备列表
};
//主机类(一个主机支持两个通道)
class Machine
{
public:
	Channel channel_a;			//第一个通道
	Channel channel_b;			//第二个通道
	bool channel_a_enable;		//第一个通道使能
	bool channel_b_enable;		//第二个通道使能
	bool snap_or_aio;			//(false中维抓拍相机)|(true中维智能相机)
	bool a_aux_camera;			//第一通道是否有辅机
	bool b_aux_camera;			//第二通道是否有辅机
	bool a_relay_enable;			//第一通道是否有网络继电器
	bool b_relay_enable;			//第二通道是否有网络继电器
	bool second_relay;			//是否有两个网络继电器
	bool one_way;				//同口进出
};


/*
 * 车辆信息类 Vehicle
 */
class Vehicle
{
public:
	std::string plate;		//车牌 UTF8
	std::string pcolor;	//车牌颜色(blue, yellow, white, black, green)
	std::string brand;		//车辆品牌
	std::string type;		//车型
	std::string color;		//车身颜色
	std::string path;		//图片路径
public:
	Vehicle()
	{
		plate = pcolor = brand = type = color = path = "";
	}
	void clear()
	{
		plate = pcolor = brand = type = color = path = "";
	}
};

bool gbk2utf8(std::string gbk, std::string &utf8);
bool utf8togb2312(std::string utf8, std::string &gb2312);
int gb2312toutf8(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
int utf8togb2312(char *inbuf, size_t inlen, char *outbuf, size_t outlen);
int code_convert(char *from_charset, char *to_charset, char *inbuf,
                 size_t inlen, char *outbuf, size_t outlen);

#endif
