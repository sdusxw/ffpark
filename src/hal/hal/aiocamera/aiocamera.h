#ifndef _BOON_AIO_CAMERA_H
#define _BOON_AIO_CAMERA_H

#include "common.h"
#include "bglobal.h"
#include "net_tcp.h"

/*
 * 智能机初始化
 * 1. 初始化相机
 */
bool aio_ipc_start();
/*
 * 落杆命令
 */
bool aio_close_door(std::string channel_id, std::string in_out);
/*
 * 开闸命令
 * flag: once表示开一次，keep表示一直开
 */
bool aio_open_door(std::string channel_id, std::string in_out,
		std::string flag);
/*
 * 手动抓拍
 * flag: 目前只支持once
 */
bool aio_snap_shot(std::string channel_id, std::string in_out,
		std::string flag);

/*
 * A通道发送车辆通过信息
 */
void aio_send_car_pass_a();

/*
 * B通道发送车辆通过信息
 */
void aio_send_car_pass_b();
/*
 * A通道推送车辆识别信息
 */
void aio_send_car_come_a();
/*
 * B通道推送车辆识别信息
 */
void aio_send_car_come_b();

/*抓拍相机类 AioCamera
 *完成
 * 1. 连接相机TCP8127端口设置上传服务器
 * 2. 打开本机服务器对应端口，等待上传
 * 3. 接收到无车牌则调用Boon算法识别
 * 4. 保存图片
 * 5. 组合结果
 */
class AioCamera
{
public:
	std::string camera_ip;
	std::string username;
	std::string password;
	std::string camera_label;	// A表示入口主机；AA表示入口辅机；B表示出口主机；BA表示出口辅机
	bool initialize();
	int listen_port;						//端口分配：A入口主机2350，AA入口辅机2351，B出口主机2352，BA出口辅机2353
public:
	NetTcpServer	tcp_server;	//Tcp接收器
};

//智能相机服务端侦听线程
void * aio_camera_loop(void* para);

#endif
