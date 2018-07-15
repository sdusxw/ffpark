#ifndef BOON_GLOBAL_H
#define BOON_GLOBAL_H
/*
 * 全局变量声明文件
 */
#include "common.h"
#include "net_udp.h"

extern UdpSender udp_sender;	//与Bcenter通讯专用udp发射器

extern UdpReceiver udp_receiver;	//与Bcenter通讯专用udp接收器

extern bool g_bipc_config_done;	//全局设备参数初始化

extern Machine g_machine;			//全局设备配置类

extern std::string g_ip_addr;			//本机IP地址

extern Vehicle g_channel_a_main_vehicle;	//A通道主相机识别结果

extern Vehicle g_channel_a_aux_vehicle;		//A通道辅相机识别结果

extern Vehicle g_channel_b_main_vehicle;	//B通道主相机识别结果

extern Vehicle g_channel_b_aux_vehicle;		//B通道辅相机识别结果

extern bool open_in_queue_a;							//A通道防跟车队列机制

extern bool open_in_queue_b;							//B通道防跟车队列机制

#endif
